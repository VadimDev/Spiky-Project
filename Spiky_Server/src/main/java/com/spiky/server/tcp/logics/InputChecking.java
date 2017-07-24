/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp.logics;

import com.spiky.server.protomodels.MessageModels;
import com.spiky.server.protomodels.RegistrationLoginModels;
import io.netty.channel.ChannelHandlerContext;
import org.hibernate.Query;
import org.hibernate.Session;

import java.util.List;
import java.util.Map;

import static com.spiky.server.ServerMain.HIBER_SESSION;
import static com.spiky.server.ServerMain.captchaBank;
import static com.spiky.server.utils.Descriptors.*;

public class InputChecking {
    public InputChecking(ChannelHandlerContext ctx, MessageModels.Wrapper wrapper) {

        Session session = ctx.channel().attr(HIBER_SESSION).get();

        if(wrapper.getInputChecking().hasField(getCaptcha_ich)) {
            ctx.writeAndFlush(new Registration().getCaptcha());
        } else if(wrapper.getInputChecking().hasField(login_ich)) {
            ctx.writeAndFlush(check(session, wrapper.getInputChecking().getLogin(), "login"));
        } else if(wrapper.getInputChecking().hasField(mail_ich)) {
            ctx.writeAndFlush(check(session, wrapper.getInputChecking().getMail(), "mail"));
        } else if(wrapper.getInputChecking().hasField(captcha_ich)) {
            ctx.writeAndFlush(check(session, wrapper.getInputChecking().getCaptcha(), "captcha"));
        }
    }

    private MessageModels.Wrapper check(Session session, String data, String type) {
        if(type.equals("login")) {
            Query query = session.createQuery("SELECT login FROM UserModel WHERE login = :str ");
            List users = query.setParameter("str", data).list();

            if(!users.isEmpty()) {
                RegistrationLoginModels.InputChecking inputChecking = RegistrationLoginModels.InputChecking.newBuilder()
                        .setLoginCheckStatus(false) // no valid
                        .build();

                return MessageModels.Wrapper.newBuilder().setInputChecking(inputChecking).build();
            } else {
                RegistrationLoginModels.InputChecking inputChecking = RegistrationLoginModels.InputChecking.newBuilder()
                        .setLoginCheckStatus(true) // valid
                        .build();
                return MessageModels.Wrapper.newBuilder().setInputChecking(inputChecking).build();
            }
        } else if (type.equals("mail")) {
            Query query = session.createQuery("SELECT mail FROM UserModel WHERE mail = :str ");
            List mails = query.setParameter("str", data).list();

            if(!mails.isEmpty()) {
                RegistrationLoginModels.InputChecking inputChecking = RegistrationLoginModels.InputChecking.newBuilder()
                        .setMailCheckStatus(false)
                        .build();
                return MessageModels.Wrapper.newBuilder().setInputChecking(inputChecking).build();
            } else {
                RegistrationLoginModels.InputChecking inputChecking = RegistrationLoginModels.InputChecking.newBuilder()
                        .setMailCheckStatus(true)
                        .build();
                return MessageModels.Wrapper.newBuilder().setInputChecking(inputChecking).build();
            }
        } else if (type.equals("captcha")) {
            boolean challengeFind = false;
            synchronized (captchaBank) {
                for (Map.Entry<Long, String> entry : captchaBank.entrySet())
                    if (entry.getValue().equals(data)) challengeFind = true;

                if (challengeFind) {
                    RegistrationLoginModels.InputChecking inputChecking = RegistrationLoginModels.InputChecking.newBuilder()
                            .setCaptchaCheckStatus(true)
                            .build();
                    return MessageModels.Wrapper.newBuilder().setInputChecking(inputChecking).build();
                } else {
                    RegistrationLoginModels.InputChecking inputChecking = RegistrationLoginModels.InputChecking.newBuilder()
                            .setCaptchaCheckStatus(false)
                            .build();
                    return MessageModels.Wrapper.newBuilder().setInputChecking(inputChecking).build();
                }
            }
        }
        return null;
    }
}
