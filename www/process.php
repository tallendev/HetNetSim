<?php
switch($_SERVER['REQUEST_METHOD'])
{
    case 'POST':
        $str = $_POST['problem'];
        error_log($str, 0);
        $a = cppMain($str);
        error_log($a, 0); 
        $response = array();
        $response['success'] = true;
        $response['posted'] = $a;
        echo json_encode($response);
        break;
}
?>
