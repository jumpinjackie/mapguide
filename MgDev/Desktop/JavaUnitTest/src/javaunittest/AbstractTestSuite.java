/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package javaunittest;

import java.util.ArrayList;

/**
 *
 * @author Jackie Ng
 */
public abstract class AbstractTestSuite {

    protected int _passes;

    protected ArrayList<Exception> _failures;

    protected AbstractTestSuite(){
        _passes=  0;
        _failures = new ArrayList<Exception>();
    }

    public int getPasses() { return _passes; }

    public ArrayList<Exception> getFailures() { return _failures; }

    protected abstract void setup();

    protected abstract void teardown();

    protected abstract void runTests();

    public void execute() {
        setup();
        try {
            runTests();
        } finally {
            teardown();
        }
    }
}
