<?php

define( 'MYSQL_HOST', "localhost" );
define( 'MYSQL_DB', "" );
define( 'MYSQL_USER', "" );
define( 'MYSQL_PW', "" );

define('TYPE_START', 0);
define('TYPE_HOURLY', 1);
define('TYPE_DAILY', 2);
define('TYPE_MONTHLY', 3);
define('TYPE_LAST', 4);
define('TYPE_DEFAULT', TYPE_HOURLY);

date_default_timezone_set("Europe/Berlin");
