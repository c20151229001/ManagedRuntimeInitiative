/*
 * Copyright 2003 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 */

/**
 * @test
 * @bug 4796166
 * @summary Linger interval delays usage of released file descriptor
 */

import java.net.*;
import java.io.*;

public class LingerTest {

    static class Sender implements Runnable {
        Socket s;

        public Sender(Socket s) {
            this.s = s;
        }

        public void run() {
            System.out.println ("Sender starts");
            try {
                s.getOutputStream().write(new byte[128*1024]);
            }
            catch (IOException ioe) {
            }
            System.out.println ("Sender ends");
        }
    }

    static class Closer implements Runnable {
        Socket s;

        public Closer(Socket s) {
            this.s = s;
        }

        public void run() {
            System.out.println ("Closer starts");
            try {
                s.close();
            }
            catch (IOException ioe) {
            }
            System.out.println ("Closer ends");
        }
    }

    static class Another implements Runnable {
        int port;
        long delay;
        boolean connected = false;

        public Another(int port, long delay) {
            this.port = port;
            this.delay = delay;
        }

        public void run() {
            System.out.println ("Another starts");
            try {
                Thread.currentThread().sleep(delay);
                Socket s = new Socket("localhost", port);
                synchronized (this) {
                    connected = true;
                }
                s.close();
            }
            catch (Exception ioe) {
                ioe.printStackTrace();
            }
            System.out.println ("Another ends");
        }

        public synchronized boolean connected() {
            return connected;
        }
    }

    public static void main(String args[]) throws Exception {
        ServerSocket ss = new ServerSocket(0);

        Socket s1 = new Socket("localhost", ss.getLocalPort());
        Socket s2 = ss.accept();


        // setup conditions for untransmitted data and lengthy
            // linger interval
            s1.setSendBufferSize(128*1024);
        s1.setSoLinger(true, 30);
        s2.setReceiveBufferSize(1*1024);

        // start sender
            Thread thr = new Thread(new Sender(s1));
        thr.start();

        // another thread that will connect after 5 seconds.
            Another another = new Another(ss.getLocalPort(), 5000);
        thr = new Thread(another);
        thr.start();

        // give sender time to queue the data
            Thread.currentThread().sleep(1000);

        // close the socket asynchronously
            (new Thread(new Closer(s1))).start();

        // give another time to run
            Thread.currentThread().sleep(10000);

        // check that another is done
            if (!another.connected()) {
            throw new RuntimeException("Another thread is blocked");
        }
        System.out.println ("Main ends");

    }

}
