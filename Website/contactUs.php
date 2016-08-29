<html>
	<body>
	<?php
		include($_SERVER['DOCUMENT_ROOT']."/includes/variables.php");
		$fromHeader = "From: ". $_POST['email'] . " (" . $_POST['student'] .")";
		mail($EmailAdmin,"VM Comment ".$_POST['student'] , "From: ". $_POST['email']. "(".$_POST['name'].")\n\n".$_POST['text']."\n\n"."Sent: ".date('h:i:sa d-m-Y'),$fromHeader);
	?>
		<script type="text/javascript">
			alert('Email sent')
			window.location.replace('Contact.php')
		</script>
	</body>
</html>