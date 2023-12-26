#ifndef HASH_HPP
#define HASH_HPP

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdexcept>

class Hash 
{
public:

    static std::string ConvertToHex(const unsigned char* digest, size_t length) 
    {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');

        for (size_t i = 0; i < length; ++i) 
            ss << std::setw(2) << static_cast<unsigned int>(digest[i]);
        
        return ss.str();
    }

    static std::string GenerateMD5Hash(const std::string& input) 
    {
        unsigned char result[MD5_DIGEST_LENGTH];
        MD5(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), result);
        return ConvertToHex(result, MD5_DIGEST_LENGTH);
    }

    static std::string GenerateSHAHash(const std::string& input) 
    {
        unsigned char result[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), result);
        return ConvertToHex(result, SHA256_DIGEST_LENGTH);
    }

    static std::string GenerateChecksum(const std::string& filePath) 
    {
        unsigned char result[SHA256_DIGEST_LENGTH];
        std::ifstream file(filePath, std::ifstream::binary);

        SHA256_CTX sha256;
        SHA256_Init(&sha256);

        if (file) 
        {
            char buffer[1024];
            while (file.read(buffer, sizeof(buffer))) 
                SHA256_Update(&sha256, buffer, file.gcount());
            
            SHA256_Update(&sha256, buffer, file.gcount());
        }
        SHA256_Final(result, &sha256);
        return ConvertToHex(result, SHA256_DIGEST_LENGTH);
    }

    static std::string GeneratePasswordHashWithSalt(const std::string& text) 
    {
        try 
        {
            auto salt = GenerateSalt();
            return GenerateSaltedHash(text, salt);
        }
        catch (const std::exception& e) 
        {
            return "";
        }
    }

    static std::string GenerateSaltedHash(const std::string& textToHash, const std::vector<unsigned char>& salt) 
    {
        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int hashLen;

        const EVP_MD* algorithm = EVP_md5();
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, algorithm, nullptr);
        EVP_DigestUpdate(ctx, salt.data(), salt.size());
        EVP_DigestUpdate(ctx, textToHash.c_str(), textToHash.size());
        EVP_DigestFinal_ex(ctx, hash, &hashLen);
        EVP_MD_CTX_free(ctx);

        return ConvertToHex(hash, hashLen);
    }

    static std::vector<unsigned char> GenerateSalt() 
    {
        const int saltLength = 16;
        std::vector<unsigned char> salt(saltLength);

        if (!RAND_bytes(salt.data(), saltLength)) 
            throw std::runtime_error("Failed to generate random salt");
        
        return salt;
    }

    static bool ValidatePassword(const std::string& originalPassword, const std::string& storedPasswordHash) 
    {
        std::vector<std::string> parts = SplitString(storedPasswordHash, ':');

        if (parts.size() != 3)
            throw std::invalid_argument("Invalid stored password hash format.");

        int iterations = std::stoi(parts[0]);
        std::vector<unsigned char> salt = ConvertToBytes(parts[1]);
        std::vector<unsigned char> storedHash = ConvertToBytes(parts[2]);

        std::vector<unsigned char> originalPasswordHash = GeneratePBEHash(originalPassword, iterations, salt, storedHash.size());

        return CompareHashes(storedHash, originalPasswordHash);
    }

    static std::vector<unsigned char> GeneratePBEHash(const std::string& password, int iterations, const std::vector<unsigned char>& salt, int keyLength) 
    {
        std::vector<unsigned char> key(keyLength);
        const EVP_MD* algorithm = EVP_sha1();

        if (!PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), salt.data(), salt.size(), iterations, algorithm, keyLength, key.data())) 
            throw std::runtime_error("PKCS5_PBKDF2_HMAC failed");
        
        return key;
    }

    static std::vector<unsigned char> ConvertToBytes(const std::string& hex) 
    {
        std::vector<unsigned char> bytes;

        for (unsigned int i = 0; i < hex.length(); i += 2) 
        {
            std::string byteString = hex.substr(i, 2);
            unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
            bytes.push_back(byte);
        }

        return bytes;
    }

    static std::vector<std::string> SplitString(const std::string& str, char delimiter) 
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);

        while (std::getline(tokenStream, token, delimiter)) 
            tokens.push_back(token);
        
        return tokens;
    }

    static bool CompareHashes(const std::vector<unsigned char>& hash1, const std::vector<unsigned char>& hash2)
    {
        if (hash1.size() != hash2.size()) 
            return false;

        for (size_t i = 0; i < hash1.size(); ++i) 
        {
            if (hash1[i] != hash2[i]) 
                return false;
        }

        return true;
    }
};

#endif // !HASH_HPP