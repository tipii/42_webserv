<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Form Response</title>
    <link href="style.css" rel="stylesheet">
</head>
<body>
    <div class="container">
        <h1>Hello, <?php echo htmlspecialchars($_POST['name']); ?>.</h1>
    </div>
</body>
</html>