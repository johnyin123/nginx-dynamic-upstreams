<?php 
$json_arr = array('WebName'=>'PHP网站开发教程网','WebSite'=>'http://www.jb51.net'); 
$php_json = json_encode($json_arr); 
?> 
<script type="text/javascript"> 
var php_json = <?=$php_json?>; 
</script> 
<script type="text/javascript"> 
function php_json_dis(php_json) 
{ 
	alert(php_json.WebName); 
	alert(php_json.WebSite); 
} 
php_json_dis(php_json); 
</script> 

