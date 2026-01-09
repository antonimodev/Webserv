<?php
header("Content-Type: text/plain");

echo "--- PHP CGI INPUT TEST ---\n";

$method = getenv('REQUEST_METHOD');
echo "Received method: $method\n";

if ($method === 'POST') {
    $entityBody = file_get_contents('php://input');
    echo "Received body:\n'$entityBody'\n";
} else {
    echo "Try with a POST.\n";
}
?>