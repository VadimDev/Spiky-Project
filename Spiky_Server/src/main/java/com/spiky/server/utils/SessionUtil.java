/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.utils;

import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.hibernate.cfg.Configuration;
import org.hibernate.service.ServiceRegistry;
import org.hibernate.service.ServiceRegistryBuilder;

public class SessionUtil {
    private final SessionFactory factory;

    public SessionUtil() {
        Configuration configuration = new Configuration();
        configuration.configure();
        ServiceRegistryBuilder srBuilder = new ServiceRegistryBuilder();
        srBuilder.applySettings(configuration.getProperties());
        ServiceRegistry serviceRegistry = srBuilder.buildServiceRegistry();
        factory = configuration.buildSessionFactory(serviceRegistry);
    }

    public Session getSession() {
        return getInstance().factory.openSession();
    }

    private static SessionUtil getInstance() {
        return new SessionUtil();
    }
}
