/*
 * Copyright (c) 2017, Vadim Petrov - MIT License
 */

package com.spiky.server.tcp.logics;

import java.util.Collections;
import java.util.Map;
import java.util.NavigableMap;
import java.util.TreeMap;

public class PlayerState {

    private String team;
    private NavigableMap<Long,Position> positionByTime = Collections.synchronizedNavigableMap(new TreeMap<>());
    private long lastUpdateTime;

    PlayerState() {
        startCleaner();
    }

    public class Position {
        Location location;
        Rotation rotation;

        Position(Location location, Rotation rotation) {
            this.location = location;
            this.rotation = rotation;
        }
    }

    /* All that is older than five seconds to delete! */
    private void startCleaner() {
        new Thread(() -> {
            long lifetime = 5000;
            while (true) {
                try {
                    if(positionByTime.size() <= 0) continue;

                    lastUpdateTime = positionByTime.lastEntry().getKey();

                    Thread.sleep(5000);

                    /* If the position has not changed in 5 seconds, leave one item and update its time */
                    if(positionByTime.lastEntry().getKey() == lastUpdateTime) {
                        long ms = System.currentTimeMillis();
                        lastUpdateTime = ms;

                        Position old_pos = positionByTime.lastEntry().getValue();
                        positionByTime.clear();
                        positionByTime.put(ms, old_pos);
                    } else {
                        positionByTime.entrySet().removeIf(e -> System.currentTimeMillis() - e.getKey() > lifetime);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    public class Location {
        private int x, y, z;

        public Location(int x, int y, int z) {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public int getX() {
            return x;
        }

        public void setX(int x) {
            this.x = x;
        }

        public int getY() {
            return y;
        }

        public void setY(int y) {
            this.y = y;
        }

        public int getZ() {
            return z;
        }

        public void setZ(int z) {
            this.z = z;
        }
    }

    public class Rotation {
        private int pitch, roll, yaw;

        public Rotation(int pitch, int roll, int yaw) {
            this.pitch = pitch;
            this.roll = roll;
            this.yaw = yaw;
        }

        public int getPitch() {
            return pitch;
        }

        public void setPitch(int pitch) {
            this.pitch = pitch;
        }

        public int getRoll() {
            return roll;
        }

        public void setRoll(int roll) {
            this.roll = roll;
        }

        public int getYaw() {
            return yaw;
        }

        public void setYaw(int yaw) {
            this.yaw = yaw;
        }
    }

    Map.Entry<Long, Position> getLastPosition() {
        return positionByTime.lastEntry();
    }

    void addPosition(long time, Position position) {
        positionByTime.put(time, position);
    }

    public Map.Entry<Long, Position> getClosestMs(long ms) {
        // todo Set sensitivity, for example, not older than 2 seconds
        return positionByTime.lowerEntry(ms);
    }

    public String getTeam() {
        return team;
    }

    public void setTeam(String team) {
        this.team = team;
    }
}
