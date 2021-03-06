/*
 * @test
 * @bug     6498300
 *
 * @summary regression: parent loggers are not properly registered
 * @author  ss45998
 *
 * @build ParentLoggersTest
 * @run main ParentLoggersTest
 */

/*
 * There are several original tests which were failed when
 * this regression was introduced. This is an extra test
 * to ensure that the parent loggers with the defined
 * .level property are implicitly registered.
 */

import java.util.*;
import java.io.*;
import java.util.logging.*;

public class ParentLoggersTest {
    static final LogManager  logMgr     = LogManager.getLogManager();
    static final PrintStream out        = System.out;

    static final boolean     PASSED     = true;
    static final boolean     FAILED     = false;
    static final String      MSG_PASSED = "ParentLoggersTest: passed";
    static final String      MSG_FAILED = "ParentLoggersTest: failed";

    /* Properties */
    static final String TST_SRC_PROP    = "test.src";
    static final String CFG_FILE_PROP   = "java.util.logging.config.file";
    static final String LM_PROP_FNAME   = "ParentLoggersTest.props";

    /* Logger names */
    static final String PARENT_NAME_1   = "myParentLogger";
    static final String PARENT_NAME_2   = "abc.xyz.foo";
    static final String LOGGER_NAME_1   = PARENT_NAME_1 + ".myLogger";
    static final String LOGGER_NAME_2   = PARENT_NAME_2 + ".myBar.myLogger";

    public static void main(String args[]) throws Exception {
        String tstSrc = System.getProperty(TST_SRC_PROP);
        File   fname  = new File(tstSrc, LM_PROP_FNAME);
        String prop   = fname.getCanonicalPath();
        System.setProperty(CFG_FILE_PROP, prop);
        logMgr.readConfiguration();

        System.out.println();
        if (checkLoggers() == PASSED) {
            System.out.println(MSG_PASSED);
        } else {
            System.out.println(MSG_FAILED);
            throw new Exception(MSG_FAILED);
        }
    }

    public static Vector getDefaultLoggerNames() {
        Vector expectedLoggerNames = new Vector(0);

        // LogManager always creates two loggers:
        expectedLoggerNames.addElement("");       // root   logger: ""
        expectedLoggerNames.addElement("global"); // global logger: "global"
        return expectedLoggerNames;
    }

    /* Check: getLoggerNames() must return correct names
     *        for registered loggers and their parents.
     * Returns boolean values: PASSED or FAILED
     */
    public static boolean checkLoggers() {
        String failMsg = "# checkLoggers: getLoggerNames() returned unexpected loggers";
        Vector expectedLoggerNames = getDefaultLoggerNames();

        // Create the logger LOGGER_NAME_1
        Logger logger1 = Logger.getLogger(LOGGER_NAME_1);
        expectedLoggerNames.addElement(PARENT_NAME_1);
        expectedLoggerNames.addElement(LOGGER_NAME_1);

        // Create the logger LOGGER_NAME_2
        Logger logger2 = Logger.getLogger(LOGGER_NAME_2);
        expectedLoggerNames.addElement(PARENT_NAME_2);
        expectedLoggerNames.addElement(LOGGER_NAME_2);

        Enumeration returnedLoggersEnum = logMgr.getLoggerNames();
        Vector      returnedLoggerNames = new Vector(0);
        while (returnedLoggersEnum.hasMoreElements()) {
            returnedLoggerNames.addElement(returnedLoggersEnum.nextElement());
        };

        return checkNames(expectedLoggerNames, returnedLoggerNames, failMsg);
    }

    // Returns boolean values: PASSED or FAILED
    private static boolean checkNames(Vector expNames,
                                      Vector retNames,
                                      String failMsg) {
        boolean status = PASSED;

        if (expNames.size() != retNames.size()) {
            status = FAILED;
        } else {
            boolean checked[] = new boolean[retNames.size()];
            for (int i = 0; i < expNames.size(); i++) {
                 int j = 0;
                for (; j < retNames.size(); j++) {
                    if (!checked[j] &&
                        expNames.elementAt(i).equals(retNames.elementAt(j))) {
                        checked[j] = true;
                        break;
                    }
                }
                if (j >= retNames.size()) {
                    status = FAILED;
                    break;
                }
            }
        }
        if (!status) {
            printFailMsg(expNames, retNames, failMsg);
        }
        return status;
    }

    private static void printFailMsg(Vector expNames,
                                     Vector retNames,
                                     String failMsg) {
        out.println();
        out.println(failMsg);
        if (expNames.size() == 0) {
            out.println("# there are NO expected logger names");
        } else {
            out.println("# expected logger names (" + expNames.size() + "):");
            for (int i = 0; i < expNames.size(); i++) {
               out.println(" expNames[" + i + "] = " + expNames.elementAt(i));
            }
        }
        if (retNames.size() == 0) {
            out.println("# there are NO returned logger names");
        } else {
            out.println("# returned logger names (" + retNames.size() + "):");
            for (int i = 0; i < retNames.size(); i++) {
               out.println("  retNames[" + i + "] = " + retNames.elementAt(i));
            }
        }
    }
}
