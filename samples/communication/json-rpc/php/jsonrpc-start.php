<?php 
  // create a new instance of Services_JSON
  require_once('json.php');
  $json_utils = new Services_JSON(SERVICES_JSON_LOOSE_TYPE);
  
  // accept incoming POST data
  
  $json_in = $json_utils->decode($GLOBALS['HTTP_RAW_POST_DATA']);

  $json_out = "";
     
  $json_method = $json_in["method"];
  $json_args = $json_in["params"];
  if( !$json_method || !$json_args)
  {
    header ("Content-type:text/plain");
    echo $json_utils->encode($json_in) . " - is not JSON v.1.1 call";
    exit();
  }
 
  $json_export_list = array();
  
  function external_method()
  {
		global $json_export_list;
		
		$argc = func_num_args();
		for ($i = 0; $i < $argc; $i++)
			$json_export_list[] = func_get_arg($i);
  }
?>