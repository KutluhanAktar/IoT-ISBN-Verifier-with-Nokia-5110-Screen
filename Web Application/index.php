<?php

require_once "ISBN_Registration_Class.php";

$DBServerName = "[_Sever_Name_]";
$DBUserName = "[_User_Name_]";
$DBPassword = "[_Password_]";
$DBName = "[_Database_Name_]";

$conn = mysqli_connect($DBServerName, $DBUserName, $DBPassword, $DBName);

$database_table = "[_Database_Table_Name_]";

$user = new ISBN_Info();
$user->defineUser($conn, $database_table);

if(isset($_GET['ISBN']) && !empty($_GET['ISBN'])){
	$user->arduino_response($_GET['ISBN']);
}

if(isset($_POST['ISBN']) && isset($_POST['name']) && isset($_POST['page']) && isset($_POST['author'])){
	$ISBN = strip_tags($_POST['ISBN']);
	$name = strip_tags($_POST['name']);
	$page = strip_tags($_POST['page']);
	$author = strip_tags($_POST['author']);
	
	$user->saveNewBook($ISBN, $name, $page, $author);
}

if(isset($_GET['delete'])){
	$user->deleteEntry($_GET['delete']);
}

?>
<!DOCTYPE html>
<html>
<head>

<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<meta name="theme-color" content="#ff5c33">
<meta name="apple-mobile-web-app-status-bar-style" content="#ff5c33">
<!-- Title -->
<title>ISBN Registration</title>
<!-- Favicon -->
<link rel="icon" type="image/png" sizes="36x36" href="icon.png">
<link rel="icon" type="image/png" sizes="48x48" href="icon.png">
<!-- Stylesheet -->
<link rel="stylesheet" type="text/css" href="ISBN_Registration_Style.css"></link>
<!-- Font -->
<link href="https://fonts.googleapis.com/css?family=Bangers&display=swap" rel="stylesheet">
<!-- jQuery -->
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>

</head>
<body>

<div class="create">
<form method="POST" action="">
<h2>New Entry</h2>
<label>ISBN: </label><input type="text" name="ISBN" placeholder="Enter ISBN..."></input><br><br>
<label>Name: </label><input type="text" name="name" placeholder="Enter name..."></input><br><br>
<label>Page: </label><input type="text" name="page" placeholder="Enter page number..."></input><br><br>
<label>Author: </label><input type="text" name="author" placeholder="Enter author..."></input>
<button type="submit" name="submit">Create +</button>
</form>
</div>

<div class="books">
<table>
  <tr>
    <th>ISBN</th>
    <th>Name</th>
    <th>Page</th>
    <th>Author</th>
    <th>Delete</th>
  </tr>
  
<?php $user->writeBooks(); ?>

</table>
</div>


<script>
  function Delete(ISBN){
	  if(ISBN != ""){
		  $.ajax({
			  url: '?delete=' + ISBN,
			  type: 'GET',
			  success: () => { window.location = window.location.href; }
		  });
	  }
  }
</script>

</body>
</html>