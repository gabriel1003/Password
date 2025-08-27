#include "crypt.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

QByteArray cr_randomBytes(int len) {
    QByteArray out(len, Qt::Uninitialized);
    RAND_bytes(reinterpret_cast<unsigned char*>(out.data()), len);
    return out;
}

QByteArray cr_deriveKey(const QString& master, const QByteArray& salt,
                        int keyLen, int iterations) {
    QByteArray key(keyLen, Qt::Uninitialized);
    const QByteArray pass = master.toUtf8();
    PKCS5_PBKDF2_HMAC(pass.constData(), pass.size(),
                      reinterpret_cast<const unsigned char*>(salt.constData()), salt.size(),
                      iterations, EVP_sha256(),
                      keyLen,
                      reinterpret_cast<unsigned char*>(key.data()));
    return key;
}

QString cr_sha256_hex(const QByteArray& data) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.constData()), data.size(), digest);

    static const char* kHex = "0123456789abcdef";
    QString hex; hex.resize(SHA256_DIGEST_LENGTH * 2);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        hex[2*i]   = kHex[(digest[i] >> 4) & 0xF];
        hex[2*i+1] = kHex[(digest[i]     ) & 0xF];
    }
    return hex;
}

bool cr_encrypt_aes_gcm(const QByteArray& key,
                        const QByteArray& plaintext,
                        const QByteArray& iv,
                        QByteArray& ciphertextOut,
                        QByteArray& tagOut) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;
    bool ok = true;
    int len = 0;
    int ct_len = 0;

    ciphertextOut.resize(plaintext.size());
    tagOut.resize(16);

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.constData()),
                           reinterpret_cast<const unsigned char*>(iv.constData())) != 1) ok = false;

    if (ok && EVP_EncryptUpdate(ctx,
            reinterpret_cast<unsigned char*>(ciphertextOut.data()), &len,
            reinterpret_cast<const unsigned char*>(plaintext.constData()), plaintext.size()) != 1) ok = false;
    ct_len = len;

    if (ok && EVP_EncryptFinal_ex(ctx,
            reinterpret_cast<unsigned char*>(ciphertextOut.data()) + len, &len) != 1) ok = false;
    ct_len += len;
    ciphertextOut.resize(ct_len);

    if (ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16,
            reinterpret_cast<unsigned char*>(tagOut.data())) != 1) ok = false;

    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

bool cr_decrypt_aes_gcm(const QByteArray& key,
                        const QByteArray& iv,
                        const QByteArray& ciphertext,
                        const QByteArray& tag,
                        QByteArray& plaintextOut) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;
    bool ok = true;
    int len = 0;
    int pt_len = 0;

    plaintextOut.resize(ciphertext.size());

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.constData()),
                           reinterpret_cast<const unsigned char*>(iv.constData())) != 1) ok = false;

    if (ok && EVP_DecryptUpdate(ctx,
            reinterpret_cast<unsigned char*>(plaintextOut.data()), &len,
            reinterpret_cast<const unsigned char*>(ciphertext.constData()), ciphertext.size()) != 1) ok = false;
    pt_len = len;

    if (ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16,
            const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(tag.constData()))) != 1) ok = false;

    if (ok && EVP_DecryptFinal_ex(ctx,
            reinterpret_cast<unsigned char*>(plaintextOut.data()) + len, &len) != 1) ok = false;
    pt_len += len;
    plaintextOut.resize(pt_len);

    EVP_CIPHER_CTX_free(ctx);
    return ok;
}
