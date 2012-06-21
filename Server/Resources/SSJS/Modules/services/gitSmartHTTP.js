/*** @author admin*//*	GIT ACCESS RIGHTS HANDLING.	There should be a resource declared for the git service with the appropriate authentication type defined.	For example, the following declaration enables BASIC authentication for git smart HTTP requests. 	<resources locationMatch="(?i)^/gitservice/" allow="GET,HEAD,POST,PUT,DELETE" group="gitters" authType="BASIC" realm="my Realm - BASIC"/>	The group "gitters" must exist in the solution's waDirectory file. The user and password must then be specified in all git command-line commands, 	as follows, in order for them to work:	git pull http://sergiy:123@192.168.6.23:8081/gitservice/MyRepo	Authentication errors are described by git simply as "fatal: Authentication failed".	Git's output should either be visible in a console somewhere in the studio's UI or at least be logged (piped) in an easily accessible file.	GIT HTTPS	Need to disable SSL certificate verification:		git config --global http.sslVerify false*///var			test = "";var			gitBuffer = new Buffer ( 1024 * 1000 );var			gitBufferSize = 0;function gitPadFromLeft ( str, strPad, maxLength ){	while ( str. length < maxLength )		str = strPad + str;    return str;}function gitCreateSmartPacket ( str ){	var		strHeader = ( str. length + 4 ). toString ( 16 );	strHeader = gitPadFromLeft ( strHeader, "0", 4 );	str = strHeader + str;	return str;}function runGitCommand ( command, inData ){	//var			gitWorker = new SystemWorker ( 'pwd' ); // Returns '/' as the current folder	var			gitWorker = new SystemWorker ( command );	gitWorker. setBinary ( true );		if ( inData != null )	{		// Write optional binary input for the command		gitWorker. postMessage ( inData ); // synchronous	}	gitWorker. onmessage = function ( e )	{		var		result = arguments [ 0 ]. data;		result. copy ( gitBuffer, gitBufferSize );		gitBufferSize += result. length;		console. log ( "Did read " + result. length + " bytes\n" );		//test += result;		//debugger;	}	gitWorker. onterminated = function ( )	{		exitWait ( );		//test += " the end";			}	wait ( 10000 );}function gitHandler ( request, response ){	gitBufferSize = 0;	var			urlQuery = request. urlQuery;	var			queryParams = {};	urlQuery. replace (					new RegExp ( "([^?=&]+)(=([^&]*))?", "g" ),					function ( $0, $1, $2, $3 ) { queryParams [ $1 ] = $3; } );	var			gitService = queryParams [ "service" ]; // git-upload-pack for example	if ( typeof gitService !== "undefined" )	{		gitService = gitService. slice ( 4 ); // remove the 'git-' prefix		var			gitCommand = "git " + gitService + " --stateless-rpc --advertise-refs "		var			gitRepository = "C:\\DBs\\gitTests\\WakandaGitSource"; // HARD-CODED FOR TESTS ONLY		gitCommand += gitRepository;		var			strSmartServerService = "# service=git-" + gitService + "\n";		strSmartServerService = gitCreateSmartPacket ( strSmartServerService );		strSmartServerService += "0000"; // smart HTTP flush		gitBuffer. write ( strSmartServerService, 0, 'utf8' );		gitBufferSize = strSmartServerService. length; // ASCII 		runGitCommand ( gitCommand, null );		response. contentType = "application/x-git-" + gitService + "-advertisement";		response. allowCache ( false );		response. headers [ "Expires" ] = "Fri, 01 Jan 1980 00:00:00 GMT";		response. headers [ "Pragma" ] = "no-cache";		response. headers [ "Cache-Control" ] = "no-cache, max-age=0, must-revalidate";		// Create a new buffer with the exact right size		var			gitResponseBuffer = gitBuffer. slice ( 0, gitBufferSize );		var			blobResponse = gitResponseBuffer. toBlob ( );		response. body = blobResponse;	}	else if ( request. contentType == "application/x-git-upload-pack-request" && request. rawURL. match ( /git-upload-pack$/ ) )	{		var			bodyIn = request. body; // BLOB		//bodyIn = bodyIn. slice ( 0, bodyIn. size - 1 )		var			bufferIn = bodyIn. toBuffer ( );var	strIn = bodyIn. toString ( );strIn = bufferIn. toString ( );		var			gitCommand = "git upload-pack --stateless-rpc "		var			gitRepository = "C:\\DBs\\gitTests\\WakandaGitSource"; // HARD-CODED FOR TESTS ONLY		gitCommand += gitRepository;		gitBufferSize = 0;		runGitCommand ( gitCommand, bufferIn );		response. contentType = "application/x-git-upload-pack-result";		// Create a new buffer with the exact right size		var			gitResponseBuffer = gitBuffer. slice ( 0, gitBufferSize );		var			blobResponse = gitResponseBuffer. toBlob ( );		response. body = blobResponse;			}	else if ( request. contentType == "application/x-git-receive-pack-request" && request. rawURL. match ( /git-receive-pack$/ ) )	{		var			bodyIn = request. body; // BLOB		//bodyIn = bodyIn. slice ( 0, bodyIn. size - 1 )		var			bufferIn = bodyIn. toBuffer ( );var	strIn = bodyIn. toString ( );strIn = bufferIn. toString ( );		var			gitCommand = "git receive-pack --stateless-rpc "		var			gitRepository = "C:\\DBs\\gitTests\\WakandaGitSource"; // HARD-CODED FOR TESTS ONLY		gitCommand += gitRepository;		gitBufferSize = 0;		runGitCommand ( gitCommand, bufferIn );		// Needed to make push work on non-bare repositories		gitCommand = "git --git-dir=\"" + gitRepository + "\\.git\" --work-tree=\"" + gitRepository + "\" checkout -f";		runGitCommand ( gitCommand, null );		response. contentType = "application/x-git-receive-pack-result";		// Create a new buffer with the exact right size		var			gitResponseBuffer = gitBuffer. slice ( 0, gitBufferSize );		var			blobResponse = gitResponseBuffer. toBlob ( );		response. body = blobResponse;			}	else		return "OK then, handled, sir!"}