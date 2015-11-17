<?php   
  if (! in_array($json_method, $json_export_list))
  {
		echo $json_method . " is not an external method";
    header ("Content-type:text/plain");
    exit();
  }

  // do call and report result:  
  header ("Content-type:application/json");
  echo $json_utils->encode( call_user_func_array($json_method, $json_args) );
?>
