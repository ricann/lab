<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<?php

$ip = '192.168.147.136';
$user = 'ricann';
$pass = 'rc'; 
$cmd = 'ls -al';
$newline = '<br />';


$connection = ssh2_connect($ip, 22);
if(!$connection) 
    die('Connection failed');
 
$rlt = ssh2_auth_password($connection, $user, $pass);
if($rlt == FALSE)
    die('Connection failed');
else
    print "SSH Connection successful!" . $newline;

$stream = ssh2_exec($connection, $cmd);
if($stream == FALSE)
    die("exec command failed: " . $cmd);

stream_set_blocking($stream, true); 
//echo stream_get_contents($stream); 

while($line = fgets($stream)) 
{
    flush();
    echo $line . $newline;
} 

fclose($stream);

?>
