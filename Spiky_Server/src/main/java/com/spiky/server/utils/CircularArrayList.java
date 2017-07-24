/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.utils;

import java.util.ArrayList;

public class CircularArrayList<E> extends ArrayList<E> {
    private ArrayList<E> list;
    private int maxSize;

    public CircularArrayList(int size) {
        list = new ArrayList<E> (size);
        maxSize = size;
    }

    @Override
    public int size() {
        return list.size();
    }

    @Override
    public boolean add (E objectToAdd) {
        if (list.size () > maxSize) {
            list.remove(0);
            list.add(objectToAdd);
        } else {
            list.add(objectToAdd);
        }
        return true;
    }

    @Override
    public E get(int i) {
        return list.get(i);
    }

    @Override
    public String toString() {
        String str = "";
        for (E element : list) str+= "[" + element + "]";
        return str;
    }
}
