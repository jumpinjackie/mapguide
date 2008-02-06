--TEST--
Bug #40398 (parent and self callback functions erroneously called statically)
--FILE--
<?php

class Base
{
	function __construct($msg)
	{
		echo __METHOD__ . "($msg)\n";
	}
}

class Derived_1 extends Base
{
	public function __construct()
	{
		$args = func_get_args();
		call_user_func_array(array($this, 'Base::__construct'), $args);
	}
}

class Derived_2 extends Base
{
	public function __construct()
	{
		$args = func_get_args();
		call_user_func_array(array($this, 'parent::__construct'), $args);
	}
}

class Derived_3 extends Base
{
	public function __construct()
	{
		$args = func_get_args();
		call_user_func_array('Base::__construct', $args);
	}
}

class Derived_4 extends Base
{
	public function __construct()
	{
		$args = func_get_args();
		call_user_func_array('parent::__construct', $args);
	}
}

class Derived_5 extends Base
{
	public function __construct()
	{
		$args = func_get_args();
		call_user_func_array(array('Base', '__construct'), $args);
	}
}

class Derived_6 extends Base
{
	public function __construct()
	{
		$args = func_get_args();
		call_user_func_array(array('parent', '__construct'), $args);
	}
}

new Derived_1('1');
new Derived_2('2');
new Derived_3('3');
new Derived_4('4');
new Derived_5('5');
new Derived_6('6');

?>
===DONE===
--EXPECTF--
Base::__construct(1)
Base::__construct(2)

Warning: call_user_func_array(): First argument is expected to be a valid callback, 'Base::__construct' was given in %sbug40398.php on line %d

Warning: call_user_func_array(): First argument is expected to be a valid callback, 'parent::__construct' was given in %sbug40398.php on line %d

Strict Standards: Non-static method Base::__construct() cannot be called statically, assuming $this from compatible context Derived_5 in %sbug40398.php on line %d
Base::__construct(5)

Strict Standards: Non-static method Base::__construct() cannot be called statically, assuming $this from compatible context Derived_6 in %sbug40398.php on line %d
Base::__construct(6)
===DONE===
