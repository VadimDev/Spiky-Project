/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.utils;

import com.spiky.server.ServerMain;

import javax.crypto.*;
import javax.crypto.interfaces.DHPrivateKey;
import javax.crypto.interfaces.DHPublicKey;
import javax.crypto.spec.DHParameterSpec;
import javax.crypto.spec.DHPublicKeySpec;
import javax.crypto.spec.SecretKeySpec;
import java.math.BigInteger;
import java.security.*;
import java.security.spec.InvalidKeySpecException;
import java.util.Base64;

public class Cryptography {
    private String secretKey;
    private String clientPublicKey;
    private String clientPrivateKey;
    private KeyAgreement clientKeyAgree;

    public String getSecretKey() {
        return secretKey;
    }

    public void setSecretKey(String secretKey) {
        this.secretKey = secretKey;
    }

    public String getClientPublicKey() {
        return clientPublicKey;
    }

    public void DiffieHellman_createKeys() {
        try {

            DHParameterSpec dhSkipParamSpec = new DHParameterSpec(P, G);

            // Alice creates her own DH key pair, using the DH parameters from above
            KeyPairGenerator aliceKpairGen = KeyPairGenerator.getInstance("DH");

            aliceKpairGen.initialize(dhSkipParamSpec);

            KeyPair aliceKpair = aliceKpairGen.generateKeyPair();

            DHPublicKey dhPub = (DHPublicKey)aliceKpair.getPublic();
            clientPublicKey = String.valueOf(dhPub.getY());

            DHPrivateKey dhPr = (DHPrivateKey)aliceKpair.getPrivate();
            clientPrivateKey = String.valueOf(dhPr.getX());

            // Alice creates and initializes her DH KeyAgreement object
            clientKeyAgree = KeyAgreement.getInstance("DH");
            clientKeyAgree.init(aliceKpair.getPrivate());

        } catch (InvalidAlgorithmParameterException | NoSuchAlgorithmException | InvalidKeyException e) {
            e.printStackTrace();
        }
    }

    public String DiffieHellman_createSecretKey(String bobPublicKey) {
        try {
            DHPublicKeySpec dhPubKeySpecs = new DHPublicKeySpec(new BigInteger(bobPublicKey), P, G);
            KeyFactory kf = KeyFactory.getInstance("DH");
            DHPublicKey bobPubKey = (DHPublicKey) kf.generatePublic(dhPubKeySpecs);

            clientKeyAgree.doPhase(bobPubKey, true);

            byte[] aliceSecret = clientKeyAgree.generateSecret();
            byte[] encodedBytes = Base64.getEncoder().encode(aliceSecret);

            String source_key = new String(encodedBytes);
            return source_key.substring(0, 16);
        } catch (NoSuchAlgorithmException | InvalidKeySpecException | InvalidKeyException e) {
            e.printStackTrace();
        }
        return null;
    }

    public byte[] Crypt(byte[] source, String key) {
        if(ServerMain.bEnableCrypto) {
            try {
                Cipher cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");
                SecretKeySpec skeySpec = new SecretKeySpec(key.getBytes(), "AES");
                cipher.init(Cipher.ENCRYPT_MODE, skeySpec);
                return cipher.doFinal(source);
            } catch (InvalidKeyException | NoSuchPaddingException | NoSuchAlgorithmException | IllegalBlockSizeException | BadPaddingException e) {
                e.printStackTrace();
            }
        } else {
            return source;
        }
        return null;
    }

    public byte[] Decrypt(byte[] cryptogram, String key) {

        //System.out.println("ServerMain.bEnableCrypto: " + ServerMain.bEnableCrypto);

        if(ServerMain.bEnableCrypto) {
            try {
                Cipher cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");
                SecretKeySpec skeySpec = new SecretKeySpec(key.getBytes(), "AES");
                cipher.init(Cipher.DECRYPT_MODE, skeySpec);
                return cipher.doFinal(cryptogram);
            } catch (IllegalBlockSizeException | BadPaddingException | NoSuchPaddingException | NoSuchAlgorithmException | InvalidKeyException e) {
                e.printStackTrace();
            }
        } else {
            return cryptogram;
        }
        return null;
    }

    // The 1024 bit Diffie-Hellman modulus values used by SKIP
    private static final byte dh1024_p[] = {
            (byte)0xF4, (byte)0x88, (byte)0xFD, (byte)0x58, (byte)0x4E, (byte)0x49, (byte)0xDB, (byte)0xCD,
            (byte)0x20, (byte)0xB4, (byte)0x9D, (byte)0xE4, (byte)0x91, (byte)0x07, (byte)0x36, (byte)0x6B,
            (byte)0x33, (byte)0x6C, (byte)0x38, (byte)0x0D, (byte)0x45, (byte)0x1D, (byte)0x0F, (byte)0x7C,
            (byte)0x88, (byte)0xB3, (byte)0x1C, (byte)0x7C, (byte)0x5B, (byte)0x2D, (byte)0x8E, (byte)0xF6,
            (byte)0xF3, (byte)0xC9, (byte)0x23, (byte)0xC0, (byte)0x43, (byte)0xF0, (byte)0xA5, (byte)0x5B,
            (byte)0x18, (byte)0x8D, (byte)0x8E, (byte)0xBB, (byte)0x55, (byte)0x8C, (byte)0xB8, (byte)0x5D,
            (byte)0x38, (byte)0xD3, (byte)0x34, (byte)0xFD, (byte)0x7C, (byte)0x17, (byte)0x57, (byte)0x43,
            (byte)0xA3, (byte)0x1D, (byte)0x18, (byte)0x6C, (byte)0xDE, (byte)0x33, (byte)0x21, (byte)0x2C,
            (byte)0xB5, (byte)0x2A, (byte)0xFF, (byte)0x3C, (byte)0xE1, (byte)0xB1, (byte)0x29, (byte)0x40,
            (byte)0x18, (byte)0x11, (byte)0x8D, (byte)0x7C, (byte)0x84, (byte)0xA7, (byte)0x0A, (byte)0x72,
            (byte)0xD6, (byte)0x86, (byte)0xC4, (byte)0x03, (byte)0x19, (byte)0xC8, (byte)0x07, (byte)0x29,
            (byte)0x7A, (byte)0xCA, (byte)0x95, (byte)0x0C, (byte)0xD9, (byte)0x96, (byte)0x9F, (byte)0xAB,
            (byte)0xD0, (byte)0x0A, (byte)0x50, (byte)0x9B, (byte)0x02, (byte)0x46, (byte)0xD3, (byte)0x08,
            (byte)0x3D, (byte)0x66, (byte)0xA4, (byte)0x5D, (byte)0x41, (byte)0x9F, (byte)0x9C, (byte)0x7C,
            (byte)0xBD, (byte)0x89, (byte)0x4B, (byte)0x22, (byte)0x19, (byte)0x26, (byte)0xBA, (byte)0xAB,
            (byte)0xA2, (byte)0x5E, (byte)0xC3, (byte)0x55, (byte)0xE9, (byte)0x2F, (byte)0x78, (byte)0xC7
    };

    private static final byte dh512_p[] = {
            (byte)0xDA, (byte)0x58, (byte)0x3C, (byte)0x16, (byte)0xD9, (byte)0x85, (byte)0x22, (byte)0x89,
            (byte)0xD0, (byte)0xE4, (byte)0xAF, (byte)0x75, (byte)0x6F, (byte)0x4C, (byte)0xCA, (byte)0x92,
            (byte)0xDD, (byte)0x4B, (byte)0xE5, (byte)0x33, (byte)0xB8, (byte)0x04, (byte)0xFB, (byte)0x0F,
            (byte)0xED, (byte)0x94, (byte)0xEF, (byte)0x9C, (byte)0x8A, (byte)0x44, (byte)0x03, (byte)0xED,
            (byte)0x57, (byte)0x46, (byte)0x50, (byte)0xD3, (byte)0x69, (byte)0x99, (byte)0xDB, (byte)0x29,
            (byte)0xD7, (byte)0x76, (byte)0x27, (byte)0x6B, (byte)0xA2, (byte)0xD3, (byte)0xD4, (byte)0x12,
            (byte)0xE2, (byte)0x18, (byte)0xF4, (byte)0xDD, (byte)0x1E, (byte)0x08, (byte)0x4C, (byte)0xF6,
            (byte)0xD8, (byte)0x00, (byte)0x3E, (byte)0x7C, (byte)0x47, (byte)0x74, (byte)0xE8, (byte)0x33
    };

    private static final BigInteger P = new BigInteger(1, dh512_p);

    private static final BigInteger G = BigInteger.valueOf(2);
}