<?php
require_once "include/config_inc.php";
require_once "include/func_inc.php";

$dblink = SetupDB();
$qry = "SELECT * FROM gamestats WHERE gamename = \"dystopia\" ORDER BY time DESC LIMIT 1;";
$res = mysql_query( $qry, $dblink );

// no data, i'm sorrey!
if (mysql_affected_rows() == 0)
	return false;
	
if (!$res)
	die("mysql query error: ".mysql_error());	

$row = mysql_fetch_array( $res, MYSQL_BOTH );
echo $row['players']."\n";

mysql_free_result($res);
?>
