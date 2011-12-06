<?php
class tc_sendmail extends tc_base{
	function test(){
		$ar = sendmail(array(
			"to" => "me@mydomain.com",
			"from" => "test@file",
			"subject" => "Hello from unit test",
			"body" => "Hi there"
		));
		$this->assertEquals("me@mydomain.com",$ar["to"]);
		$this->assertEquals("test@file",$ar["from"]);
		$this->assertEquals("Hello from unit test",$ar["subject"]);
		$this->assertEquals("test@file",$ar["return_path"]);
		$this->assertTrue((bool)preg_match("/From: test@file/",$ar["headers"]));
		$this->assertTrue((bool)preg_match("/Content-Type: text\\/plain; charset=us-ascii/",$ar["headers"]));

		$ar = sendmail(array(
			//"to" => "me@mydomain.com",
			"from" => "test@file",
			"return_path" => "return_path@file",
			"subject" => "Hello from unit test",
			"body" => "Hi there"
		));
		$this->assertEquals("dummy@localhost",$ar["to"]);
		$this->assertEquals("return_path@file",$ar["return_path"]);

		$ar = sendmail(array(
			"to" => "me@mydomain.com",
			//"from" => "test@file",
			"subject" => "Hello from unit test",
			"body" => "Hi there"
		));
		$this->assertTrue((bool)preg_match("/From: info@somewhere/",$ar["headers"]));

		$ar = sendmail(array(
			"to" => "me@mydomain.com",
			"from" => "test@file",
			"subject" => "Hello from unit test",
			"body" => "Hi there",
			"charset" => "UTF-8",
			"mime_type" => "text/html"
		));
		$this->assertTrue((bool)preg_match("/Content-Type: text\\/html; charset=UTF-8/",$ar["headers"]));

		// pokud charset nastavime prazdny, nesmi se ve vystupu objevit
		$ar = sendmail(array(
			"to" => "me@mydomain.com",
			"from" => "test@file",
			"subject" => "Hello from unit test",
			"body" => "Hi there",
			"charset" => "",
			"mime_type" => "text/html"
		));
		$this->assertTrue((bool)preg_match("/Content-Type: text\\/html\\n/",$ar["headers"]));
	}

	function test_from_name(){
		$ar = sendmail($params = array(
			"from" => "john.doe@example.com",
			"from_name" => "John Doe",
		));
		$this->assertEquals("john.doe@example.com",$ar["from"]);
		$this->assertTrue((bool)preg_match('/From: John Doe <john.doe@example.com>/',$ar["headers"]));

		$ar = sendmail($params = array(
			"from" => "John Doe <john.doe@example.com>",
		));
		$this->assertEquals("john.doe@example.com",$ar["from"]);
		$this->assertTrue((bool)preg_match('/From: John Doe <john.doe@example.com>/',$ar["headers"]));

		$ar = sendmail(array(
			"from" => "vesela-prochazka@example.com",
			"from_name" => "Veselá Procházka",
			"charset" => "UTF-8",
		));
		$this->assertEquals("vesela-prochazka@example.com",$ar["from"]);
		$this->assertTrue((bool)preg_match('/From: =\?UTF-8\?Q\?Vesel=C3=A1_Proch=C3=A1zka\?= <vesela-prochazka@example.com>/',$ar["headers"]));

		$ar = sendmail(array(
			"from" => '"Veselá Procházka" <vesela-prochazka@example.com>',
			"charset" => "UTF-8",
		));
		$this->assertEquals("vesela-prochazka@example.com",$ar["from"]);
		$this->assertTrue((bool)preg_match('/From: =\?UTF-8\?Q\?Vesel=C3=A1_Proch=C3=A1zka\?= <vesela-prochazka@example.com>/',$ar["headers"]));
	}

	function test_to_as_array(){
		$ar = sendmail(array(
			"to" => array("me@mydomain.com","she@mydomain.com"),
		));
		$this->assertEquals("me@mydomain.com, she@mydomain.com",$ar["to"]);

		// odstraneni unique zaznamu
		$ar = sendmail(array(
			"to" => array("me@mydomain.com","she@mydomain.com","me@mydomain.com","he@mydomain.com"),
		));
		$this->assertEquals("me@mydomain.com, she@mydomain.com, he@mydomain.com",$ar["to"]);

		// odstraneni prazdneho zaznamu
		$ar = sendmail(array(
			"to" => array("me@mydomain.com","she@mydomain.com","we@mydomain.com",""),
		));
		$this->assertEquals("me@mydomain.com, she@mydomain.com, we@mydomain.com",$ar["to"]);
	}

	function test_bcc(){
		$ar = sendmail(array());
		$this->assertEquals("big.brother@somewhere.com",$ar["bcc"]); //konstanta SENDMAIL_BCC_TO

		$ar = sendmail(array(
			"bcc" => "admin@localhost"
		));
		$this->assertEquals("admin@localhost, big.brother@somewhere.com",$ar["bcc"]);

		$ar = sendmail(array(
			"bcc" => array("admin@localhost","","root@localhost","admin@localhost")
		));
		$this->assertEquals("admin@localhost, root@localhost, big.brother@somewhere.com",$ar["bcc"]);
	}

	function test_cc(){
		$ar = sendmail(array());
		$this->assertEquals("",$ar["cc"]);

		$ar = sendmail(array(
			"cc" => "cc@localhost"
		));
		$this->assertEquals("cc@localhost",$ar["cc"]);

		$ar = sendmail(array(
			"cc" => array("cc@localhost","","cc@localhost","cc2@localhost")
		));
		$this->assertEquals("cc@localhost, cc2@localhost",$ar["cc"]);
	}

	function test_legacy_parameters(){
		$ar = sendmail(array(
			"to" => "me@mydomain.com",
			"from" => "test@file",
			"subject" => "Hello from unit test",
			"body" => "Hi there",
			"body_charset" => "WINDOWS-1250",
			"body_mime_type" => "text/html"
		));
		$this->assertTrue((bool)preg_match("/Content-Type: text\\/html; charset=WINDOWS-1250/",$ar["headers"]));
	}

	function test_send_attachment(){
		$ar = sendmail(array(
			"subject" => "I am sending you my picture",

			"to" => array("friend@mydomain.com","friend@mydomain.com",""),
			"from" => "me@mydomain.com",
			"cc" => array("his.father@mydomain.com",""),
			"bcc" => array("myself@localhost"),

			"body" => "<p>sending attachment</p>",
			"body_mime_type" => "text/html",
			"body_charset" => "us-ascii",

			"attachment" => array(
				"body" => files::get_file_content("fck.gif",$err,$err_str),
				"filename" => "fck.gif",
				"mime_type" => "image/gif"
			)
		));
		$this->assertTrue((bool)preg_match("/cc: his.father@mydomain.com\\n/",$ar["headers"]));
		$this->assertTrue((bool)preg_match("/bcc: myself@localhost, big.brother@somewhere.com\\n/",$ar["headers"]));
		$this->assertTrue((bool)preg_match("/From: me@mydomain.com\\n/",$ar["headers"]));

		$this->assertTrue(strlen($ar["body"])>1000);
		$this->assertTrue((bool)preg_match("/Content-Type: text\\/html; charset=\"us-ascii\"\\n/",$ar["body"]));
		$this->assertTrue((bool)preg_match("/<p>sending attachment<\\/p>/",$ar["body"]));
		$this->assertTrue((bool)preg_match("/Content-type: image\\/gif; name=\"fck.gif\";\\n/",$ar["body"]));
		$this->assertTrue((bool)preg_match("/Content-disposition: attachment; filename=\"fck.gif\"\\n/",$ar["body"]));
	}

	function test_transfer_encoding(){
		$ar_default = sendmail(array(
			"to" => "me@mydomain.com",
			"from" => "test@file",
			"subject" => "Hello from unit test",
			"body" => "Pěkný pozdrav!\n\nZasílá Rosťa! ",
		));
		$this->assertEquals("Pěkný pozdrav!\n\nZasílá Rosťa! ",$ar_default["body"]);
		$this->assertTrue((bool)preg_match("/Content-Transfer-Encoding: 8bit/",$ar_default["headers"]));

		$ar_8bit = sendmail(array(
			"to" => "me@mydomain.com",
			"from" => "test@file",
			"subject" => "Hello from unit test",
			"body" => "Pěkný pozdrav!\n\nZasílá Rosťa! ",
			"transfer_encoding" => "8bit",
		));
		$this->assertEquals($ar_8bit,$ar_default);

		$ar_qp = sendmail(array(
			"to" => "me@mydomain.com",
			"from" => "test@file",
			"subject" => "Hello from unit test",
			"body" => "Pěkný pozdrav!\n\nZasílá Rosťa! ",
			"transfer_encoding" => "quoted-printable",
		));
		$this->assertEquals("P=C4=9Bkn=C3=BD pozdrav!\r\n\r\nZas=C3=ADl=C3=A1 Ros=C5=A5a!=20",$ar_qp["body"]);
		$this->assertTrue((bool)preg_match("/Content-Transfer-Encoding: quoted-printable/",$ar_qp["headers"]));
	}

	function test_lf_to_crlf(){
		$this->assertEquals("text",_sendmail_lf_to_crlf("text"));
		$this->assertEquals("line\r\nline",_sendmail_lf_to_crlf("line\r\nline"));
		$this->assertEquals("line\r\nline",_sendmail_lf_to_crlf("line\nline"));
		$this->assertEquals("line\r\nline\r\nline",_sendmail_lf_to_crlf("line\nline\r\nline"));
	}

	function test_quoted_printable_encode(){
		$this->assertEquals("Ahoj!",_sendmail_quoted_printable_encode("Ahoj!"));
		$this->assertEquals("110=25",_sendmail_quoted_printable_encode("110%"));
		$this->assertEquals("1 + 2 =3D 3",_sendmail_quoted_printable_encode("1 + 2 = 3"));
		
		// pokud neni tabulator na konci radky, muze zustat beze zmeny
		$this->assertEquals("test\ttab",_sendmail_quoted_printable_encode("test\ttab"));

		// na konci radky nebo textu musi byt zakodovan
		$this->assertEquals("=09",_sendmail_quoted_printable_encode("\t"));
		$this->assertEquals("test=09",_sendmail_quoted_printable_encode("test\t"));
		$this->assertEquals("test\ttab=09\r\ntest\ttab=09",_sendmail_quoted_printable_encode("test\ttab\t\r\ntest\ttab\t"));

		// pokud neni spaceulator na konci radky, muze zustat beze zmeny
		$this->assertEquals("test\tspace",_sendmail_quoted_printable_encode("test\tspace"));

		// na konci radky nebo textu musi byt zakodovan
		$this->assertEquals("=09",_sendmail_quoted_printable_encode("\t"));
		$this->assertEquals("test=09",_sendmail_quoted_printable_encode("test\t"));
		$this->assertEquals("test\tspace=09\r\ntest\tspace=09",_sendmail_quoted_printable_encode("test\tspace\t\r\ntest\tspace\t"));

		$this->assertEquals("=25",_sendmail_quoted_printable_encode("%"));

		$body = str_repeat("a",1000);
		$encoded = $this->_encode_body($body);
		$lines = explode("\r\n",$encoded);
		$this->assertEquals(14,sizeof($lines)); // 1000/76

		$body = "Banket v šermířském klubu již trval přes půlnoc, když tlustý pan Bartsch se sklenicí v ruce povstal k svému pátému a poslednímu toastu.

Tlustý pan Bartsch tentokráte připíjel dámám, našim paním a jejich vzdáleným půvabům, tlumě svůj hlas vibrující a hladký, neboť ony právě usínají, oddechujíce na loži, a potichu sténají, rozrušovány ženskými sny; tak mluvil, černý a vzpřímený, skládaje galantní periody a sladké pocty našim paním, vzácným ženám s účesem a vlečkou, kterým sloužíme.

V tu chvíli stalo se utrpení mladého Richarda již nesnesitelným a slzy stoupaly do jeho hezkých světlých očí. A tento krásný muž, aby přemohl náhlou slabost své zoufalé chvíle, položil dlaň na hořící konec svého doutníku a nechal fyzickou bolest růsti až k tichému zaúpění. Pak seděl opět bledý a vztyčený a usmíval se shovívavě k mateřsky něžným pozornostem svého znepokojeného Pylada, jenž pozbývaje soucitem rozvahy, hladil ramena svého přítele.

“Mladý Richard byl zrazen od své milenky,” šeptalo se na druhém konci stolu.";
		$encoded = _sendmail_quoted_printable_encode($body);
		$encoded = $this->_encode_body($body);


		$body = "Vase zadost o registraci .eu domeny xn--orsg-7na.eu skoncila s timto vysledkem:
Your request for the registration of the .eu domain xn--orsg-7na.eu finished with this result:

registrace domeny probehla uspesne

the registration of the domain was successfully realized

[ Na tuto zpravu NEODPOVIDEJTE, odpoved zustane neprectena. ]

Radky pro automaticke zpracovani:
PROCESS|EUDOMAINREG|xn--orsg-7na.eu|1000|
PROCESSCONTROL|eudocrpe_11442|NA|
";	
		//echo $this->_encode_body($body)."\n\n";

		$this->_encode_body($body,array("split_up_long_lines_in_spaces" => false));
		$this->_encode_body($body,array("split_up_long_lines_in_spaces" => true));

		$this->_encode_body(str_repeat(" ",100),array("split_up_long_lines_in_spaces" => true));
		$this->_encode_body(str_repeat(" ",200),array("split_up_long_lines_in_spaces" => true));
		$this->_encode_body(str_repeat(" ",100),array("split_up_long_lines_in_spaces" => false));
		$this->_encode_body(str_repeat("a",100),array("split_up_long_lines_in_spaces" => true));
		$this->_encode_body(str_repeat("a",200),array("split_up_long_lines_in_spaces" => true));
		$this->_encode_body(str_repeat("\t",200),array("split_up_long_lines_in_spaces" => true));
		//echo _sendmail_quoted_printable_encode(str_repeat("a",200)." ".str_repeat("b",200),array("split_up_long_lines_in_spaces" => true)); exit;
		$this->_encode_body(str_repeat("a",200)." ".str_repeat("b",200),array("split_up_long_lines_in_spaces" => true));
		$this->_encode_body(str_repeat("a",200)." ".str_repeat("b",200),array("split_up_long_lines_in_spaces" => false));

		//$this->_encode_body(files::get_file_content("ihned.html",$err,$err_str),array("split_up_long_lines_in_spaces" => true));

		$body = files::get_file_content("ihned.html",$err,$err_str);
		$this->_encode_body($body,array("split_up_long_lines_in_spaces" => true));
		//echo _sendmail_quoted_printable_encode($body,array("split_up_long_lines_in_spaces" => true)); exit;

		// tady testuju, ze zakodovany text neobsahuje radky pouze se znakem =
		// takove radky nemaji v textu zadny vyznam
		$encoded = $this->_encode_body("Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Nam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum. Typi non habent claritatem insitam; est usus legentis in iis qui facit eorum claritatem. Investigationes demonstraverunt lectores legere me lius quod ii legunt saepius. Claritas est etiam processus dynamicus, qui sequitur mutationem consuetudium lectorum. Mirum est notare quam littera gothica, quam nunc putamus parum claram, anteposuerit litterarum formas humanitatis per seacula quarta decima et quinta decima. Eodem modo typi, qui nunc nobis videntur parum clari, fiant sollemnes in futurum.",array("split_up_long_lines_in_spaces" => true));
		foreach(preg_split("/\r\n/",$encoded) as $line){
			$this->assertTrue($line!="=");
		}

	}

	function test_get_boundary(){
		$b1 = _sendmail_get_boundary();
		$b2 = _sendmail_get_boundary();
		$this->assertEquals(28,strlen($b1));
		$this->assertEquals(28,strlen($b2));
		$this->assertTrue($b1!=$b2);
	}

	function test_quoting_subject(){
		// s ciste ascii subjectem asi neni problem vubec
		$ar = array(
			"to" => "bar@gmail.com",
			"subject" => "A message for: Dear Mr/Mrs Bar",
			"charset" => "UTF-8",
		);
		$eml = sendmail($ar);
		$this->assertEquals("A message for: Dear Mr/Mrs Bar",$eml["subject"]);

		// ted mame non-ascii subject
		// dvojtecka a lomitko musi byt escapovane, mezera je nahrazena podtrzitkem
		$ar["subject"] = "A message for: Dear Mr/Mrs Bář_Bar";
		$eml = sendmail($ar);
		$this->assertEquals("=?UTF-8?Q?A_message_for=3A_Dear_Mr=2FMrs_B=C3=A1=C5=99=5FBar?=",$eml["subject"]);

		// pokud uz dodame zakodovany subject, bude vracen beze zmeny
		$ar["subject"] = "=?windows-1250?Q?Objedn=E1n=ED_informa=E8n=EDho_kan=E1lu?=";
		$eml = sendmail($ar);
		$this->assertEquals("=?windows-1250?Q?Objedn=E1n=ED_informa=E8n=EDho_kan=E1lu?=",$eml["subject"]);
	}

	// zakoduje body do quoted_printable a provede testy na max. delku radek
	function _encode_body($body,$options = array()){
		$this->assertTrue(strlen($body)>0);

		$body = _sendmail_lf_to_crlf($body);
		$encoded = _sendmail_quoted_printable_encode($body,$options);

		$lines = explode("\r\n",$encoded);
		foreach($lines as $line){
			$this->assertTrue(strlen($line)<=76);
		}

		$this->assertEquals($body,quoted_printable_decode($encoded));
		return $encoded;
	}
}
?>