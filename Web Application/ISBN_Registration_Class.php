<?php

class ISBN_Info{
	
	public $conn, $database_table;
	
	public function defineUser($conn, $database_table){
		$this->conn = $conn;
		$this->database_table = $database_table;
	}
	
	public function saveNewBook($isbn, $name, $page, $author){
		$Name = $name;
		$Author = $author;
		$Page = $page;
		$ISBN = $isbn;
		
		if(!empty($Name) || !empty($Author) || !empty($Page) || !empty($ISBN)){
			if(strlen($Name) < 150 || strlen($Author) < 100 || strlen($Page) < 6 || strlen($ISBN) < 50){
				$sql = "INSERT INTO `$this->database_table` (`isbn`, `name`, `author`, `page`) VALUES ('$ISBN', '$Name', '$Author', '$Page')";
				mysqli_query($this->conn, $sql);
            
			}
		}
	}
	
	public function writeBooks(){
	   $sql = "SELECT * FROM `$this->database_table` ORDER BY `id` DESC";
	   $result = mysqli_query($this->conn, $sql);
	   $check = mysqli_num_rows($result);
	   if($check < 1){
		   echo '
		     <tr>
			   <td>No Entry!</td>
			   <td>No Entry!</td>
			   <td>No Entry!</td>
			   <td>No Entry!</td>
			   <td>No Entry!</td>
			 <tr>
		   ';
	   }else{
		   while($row = mysqli_fetch_assoc($result)){
			   echo '
			     <tr>
				   <td>'.$row["isbn"].'</td>
				   <td>'.$row["name"].'</td>
				   <td>'.$row["page"].'</td>
				   <td>'.$row["author"].'</td>
				   <td><button onclick="Delete('."'".$row["isbn"]."'".')">Delete X</button></td>
				 </tr>
			   ';
		   }
	   }
	}
	
	public function deleteEntry($ISBN){
		$sql = "DELETE FROM `$this->database_table` WHERE isbn='$ISBN'";
		mysqli_query($this->conn, $sql);
		
		exit();
	}
	
	public function arduino_response($ISBN){
		$sql = "SELECT * FROM `$this->database_table` WHERE isbn = '$ISBN'";
		$result = mysqli_query($this->conn, $sql);
		$check = mysqli_num_rows($result);
		if($check > 0){
			if($row = mysqli_fetch_assoc($result)){
				$response = '%'.$row["name"].'%'.$row["page"].'%'.$row["author"].'%'.$row["id"].'%';
				echo $response;
			}
		}else{
			echo '%Not Found!%Not Found!%Not Found!%Not Found!%';
		}
		
		exit();
	}
}

?>