/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.dbmodels;

import javax.persistence.*;

@Entity
public class UserModel {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    @Column
    String login;
    @Column
    String hash;
    @Column
    String mail;

    public String getLogin() {
        return login;
    }

    public void setLogin(String login) {
        this.login = login;
    }

    public String getHash() {
        return hash;
    }

    public void setHashPass(String hash) {
        this.hash = hash;
    }

    public String getMail() {
        return mail;
    }

    public void setMail(String mail) {
        this.mail = mail;
    }

    @Override
    public String toString() {
        return "login: \"" + login + "\"\nmail: \"" + mail + "\"\nhash: \"" + hash + "\"";
    }
}