<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>A simple php page</title>
    <link href="style.css" rel="stylesheet">
</head>
<body>
    <div class="container">
        <h2>If you can see the date, php is working </h2>
        <h1><?php echo date('l jS \of F Y h:i:s A'); ?></h1>
        
        <h2>Args passed in query :</h2>
        <ul>
        <?php
            foreach ($_GET as $key => $value)
            {
                echo ("<li>" . $key . " : " . $value . "</li>");
            }

        ?>
        </ul>

        <a href="./index.php?did%20it%20worked=great&am%20i%20happy=yes">Try a get request with some args in url's query string</a>
    </div>
</body>
</html>