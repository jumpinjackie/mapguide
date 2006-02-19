#!/bin/sh

# compile xgrep and evaluator if necessary

cd ../samples/evaluator
make
cd ../xgrep
make
cd ../../functionalTesting

export TZ="CEU-5"

failures=0
here=`pwd`
# for loop gets path to all the policy.test files
# each directory is then descended into and the tester is called
# if errors are found the failures variable is set to 1
for i in `find . -name tester.xml | sed -e 's/tester.xml//;s/^.\///' | sort`
do
  ./tester $@ $here/$i || failures=1
done

# if failures variable was set then cat together the failures file
cd $here
if test $failures -gt 0;
  then find . -name failures.* -size +0 -print -exec cat \{\} \; 
fi
exit $failures
