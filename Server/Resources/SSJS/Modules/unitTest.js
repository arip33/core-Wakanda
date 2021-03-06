/*
* This file is part of Wakanda software, licensed by 4D under
*  (i) the GNU General Public License version 3 (GNU GPL v3), or
*  (ii) the Affero General Public License version 3 (AGPL v3) or
*  (iii) a commercial license.
* This file remains the exclusive property of 4D and/or its licensors
* and is protected by national and international legislations.
* In any event, Licensee's compliance with the terms and conditions
* of the applicable license constitutes a prerequisite to any use of this file.
* Except as otherwise expressly stated in the applicable license,
* such license does not include any other license or rights on this file,
* 4D's and/or its licensors' trademarks and/or other proprietary rights.
* Consequently, no title, copyright or other proprietary rights
* other than those specified in the applicable license is granted.
*/
/**
* @author Sebastien.Courvoisier@4d.com
*/
// "use strict";

/*global require,console,application,Folder,File,TextStream,include,exports*/

var run, handlers, start, stop, getReport, getXmlReport, getHtmlReport, sendReport, sendXmlReport, sendHtmlReport, getModulePath, getenv;

// Global stuff (both client and server side):

/**
 * @method run
 */
run = exports.run = function (aTestCase, aCallBack) {
    if (typeof getGlobal().Y === "undefined") {
        if (isServerSide()) {
            include(File(module.id + "/yui-combo.js"));
            include(File(module.id + "/yui-watest.js"));
            getGlobal().Y = YUI();
        }
        else {
            getGlobal().Y = YUI();
        }
    }

    if (isServerSide()) {
        currentSideName = "Server";
        currentHostName = application.httpServer.ipAddress + ":" + application.httpServer.port;
        currentUrl = "http://" + currentHostName + "/testServer?path=";
    }
    else {
        currentSideName = "Client";
        currentHostName = location.host;
        currentUrl = String(location.href).split("&");
        currentUrl = currentUrl[0];
    }
    
    var _this = this;
    if (typeof aCallBack === "function") currentCallBack = aCallBack;
    
    getGlobal().Y.use('watest', function(Y) {
        if (initTestRunnerDone === false) {
            function handleTestFail(data) {
                console.log("Test named '" + data.testName + "' failed with message: '" + data.error.message + "'.");
            }
            function handleTestPass(data) {
                console.log("Test named '" + data.testName + "' passed.");
            }
            function handleTestComplete(data) {
                console.log("Test completed.");
                currentResults = data.results;
				if (currentCallBack !== null) currentCallBack.call(_this, currentResults);
				if (typeof exitWait !== "undefined") exitWait();
            }
            Y.Test.Runner.subscribe(Y.Test.Runner.TEST_FAIL_EVENT, handleTestFail);
            Y.Test.Runner.subscribe(Y.Test.Runner.TEST_PASS_EVENT, handleTestPass);
            Y.Test.Runner.subscribe(Y.Test.Runner.COMPLETE_EVENT, handleTestComplete);
            initTestRunnerDone = true;
        }
        
        currentTestCase = null;
        currentResults = null;
        currentTestName = "";
        currentTestFileName = "";
        Y.Test.Runner.clear();
        
        if (typeof aTestCase === "string") {
            currentTestFileName = aTestCase;
            if (isServerSide()) {
                var re = /^walib/i;
                if (re.test(aTestCase)) {
                    currentUrl += application.getWalibFolder("path") + aTestCase.replace(/walib\//i,'');
                    include(File(application.getWalibFolder("path") + aTestCase.replace(/walib\//i,'')));
                }
                else {
                    currentUrl += aTestCase;
                    include(aTestCase);
                }
                if (typeof testCase === "object") {
                    currentTestName = testCase.name;
                    currentTestCase = new Y.Test.Case(testCase);
                }
            }
            else {
                var req = new XMLHttpRequest();
                req.open("GET", "http://" + location.host + "/" + aTestCase, false);
                req.send(null);
                try {
                    eval(req.responseText);
                }
                catch (e) {}
                if (typeof testCase === "object") {
                    currentTestName = testCase.name;
                    currentTestCase = new Y.Test.Case(testCase);
                }
            }
        }
        else if (typeof aTestCase === "object") {
            currentTestName = aTestCase.name;
            currentTestCase = new Y.Test.Case(aTestCase);
        }
        
        if (currentTestCase !== null) Y.Test.Runner.add(currentTestCase);
        
        if (currentTestCase !== null && typeof wait === "function") {
        	if (typeof currentTestCase._wait === "object" && typeof currentTestCase._wait.before !== "undefined") {
        		console.log("Test asks to wait " + currentTestCase._wait.before + " before run.");
        		wait(currentTestCase._wait.before);
        	}	
        }
        
        Y.Test.Runner.run();
        
        if (currentTestCase !== null && typeof wait === "function") {
        	wait(Math.max(100, 1000 * getTestCount(currentTestCase)));        	
        	if (typeof currentTestCase._wait === "object" && typeof currentTestCase._wait.after !== "undefined") {
        		console.log("Test asks to wait " + currentTestCase._wait.after + " after run.");
        		wait(currentTestCase._wait.after);
        	}	
        }
    });
    
    return this;
};

handlers = [
    {pattern: '/testReporter', file: 'reporter.js', method: 'reporterHandler'},
    {pattern: '/testServer?.*', file: 'server.js', method: 'serverHandler'},
    {pattern: '/testClient?.*', file: 'client.js', method: 'clientHandler'},
    {pattern: '/testSimple', file: 'simple.js', method: 'simpleHandler'},
    {pattern: '/testEcho/.*', file: 'echo.js', method: 'echoHandler'},
    {pattern: '/getenv', file: 'getenv.js', method: 'getenvHandler'},
    {pattern: '/unitTest\\.js', file: 'unitTest.js', method: 'unitTestHandler'}
];

/**
 * @method start
 */
start = exports.start = function () {
	handlers.forEach(
        function addHandler(handler) {
            var path = module.id + "/" + handler.file;
            
            if (File(path).exists) {
                application.addHttpRequestHandler(
                    handler.pattern, 
                    path, 
                    handler.method
                );
            }
        }
    );
};

/**
 * @method stop
 **/
stop = exports.stop = function () {
    handlers.forEach(
        function removeHandler(handler) {
            application.removeHttpRequestHandler(
                handler.pattern, 
                module.id + "/" + handler.file, 
                handler.method
            );
        }
    );
    return true;
};

/**
 * @method getReport
 */
getReport = exports.getReport = function () {
    return currentResults;
};

/**
 * @method getXmlReport
 */
getXmlReport = exports.getXmlReport = function () {
    return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + formatXmlReport(currentResults);
};

/**
 * @method getHtmlReport
 */
getHtmlReport = exports.getHtmlReport = function () {
    return formatHtmlReport(currentResults);
};

/**
 * @method sendReport
 */
sendReport = exports.sendReport = function (url, location) {
    doSendReport(getReport(), url, location);
    return this;
};

/**
 * @method sendXmlReport
 */
sendXmlReport = exports.sendXmlReport = function (url, location) {
    doSendReport(getXmlReport(), url, location);
    return this;
};

/**
 * @method sendHtmlReport
 */
sendHtmlReport = exports.sendHtmlReport = function (url, location) {
    doSendReport(getHtmlReport(), url, location);
    return this;
};

/**
 * @method getenv
 */
getenv = exports.getenv = function () {
    var envVars = {};
    if (isServerSide()) {
    	if (os.isMac || os.isLinux) {
			var myEnvWorker = new SystemWorker('/usr/bin/printenv');
		}
		else {
			var myEnvWorker = new SystemWorker('cmd /C set');
		}
		myEnvWorker.onmessage = function() {
			var resultLines = arguments[0].data.split('\n');
		    for (var i = 0, j = resultLines.length; i < j; i++) {
		    	var envVar = resultLines[i].split('=');
		    	envVars[envVar[0]] = envVar[1];
			}	
		};
		wait(250);
    }
    return envVars;
};

/**
 * @method getModulePath
 */
getModulePath = exports.getModulePath = function () {
    return module.id;
};


/* private stuff */
var initTestRunnerDone = false;
var initTestRunnerRequestHandlersDone = false;
var currentTestCase = null;
var currentCallBack = null;
var currentResults =  null;
var currentTestFileName = "";
var currentTestName = "";
var currentSideName = "Unknown";
var currentHostName = "";
var currentUrl = "";

function isServerSide () {
    return (typeof navigator !== "object");
}

function getGlobal () {
    if (isServerSide()) return application;
    return window;
}

function getTestCount (testCase) {
    var count = 0;
    for (prop in testCase) if ((prop.indexOf("test") === 0 || (prop.toLowerCase().indexOf("should") > -1 && prop.indexOf(" ") > -1 )) && getGlobal().Y.Lang.isFunction(testCase[prop])) count++;
    return count;
}

function formatXmlReport (result, depth) {
    var report = "";
    if (typeof depth === "undefined") depth = 0;
    
    if (result && result.type) {
        var nodeName = "";
        switch (result.type) {
            case "report":
                nodeName = "testsuites";
                break;
            case "testcase":
                nodeName = "testsuite";
                break;
            case "test":
                nodeName = "testcase";
                break;
        }
        report = printTabs(depth) + "<" + nodeName;	
        if (result.name) {
            if (result.name === "_name") {
                report += " name=\"name\"";
                if (result.type !== "report") report += " href=\"" + currentUrl + "#name\"";
            }
            else {
                report += " name=\"" + escapeXml(result.name) + "\"";
                if (result.type !== "report") report += " href=\"" + currentUrl + "#" + escapeXml(result.name) + "\"";
            }
        }				
        if (result.type === "test") {	
            if (result.version) report += " version=\"" + result.version + "\"";
            if (result.description) report += " description=\"" + escapeXml(result.description) + "\"";	
            if (result.result === "pass") report += " passed=\"true\"";
            else if (result.result === "ignore") report += " ignored=\"true\"";	
            else report += " passed=\"false\"";	
            report += " time=\"" + result.duration + "\"";
            report += ">\n";	
            var offset = 0;					
            if (result.result === "ignore") report += printTabs(depth + 1) + "<skipped message=\"" + escapeXml(result.message) + "\"><![CDATA[" + result.message + "]]></skipped>\n";
            else if (result.result !== "pass") report += printTabs(depth + 1) + "<failure message=\"" + escapeXml(result.message) + "\"><![CDATA[" + result.message + "]]></failure>\n";
            else report += printTabs(depth + 1) + "<success message=\"" + escapeXml(result.message) + "\"><![CDATA[" + result.message + "]]></success>\n";
        }
        else {	
            if (result.type === "report") {
                if (currentTestFileName !== "") report += " filename=\"" + currentTestFileName + "\"";
                report += " hostname=\"" + currentHostName + "\"";
                report += " href=\"" + currentUrl + "\"";
            }
            report += " passed=\"" + result.passed + "\" failures=\"" + result.failed + "\" skipped=\"" + result.ignored + "\" tests=\"" + result.total + "\">\n";
            for (var i in result) if (typeof result[i] === "object") report += formatXmlReport(result[i], depth + 1);			
            if (depth === 0) report = report.replace(/^<testsuites/, "<testsuites success=\"" + Math.round(result.passed / result.total * 100) + "%\" time=\"" + result.duration + "\"");
        }									
        report += printTabs(depth) + "</" + nodeName + ">\n";
    }
    
    return report;
}

function formatHtmlReport (result, depth) {
    var report = "";
    if (typeof depth === "undefined") depth = 0;
    
    if (result && result.type) {
        var nodeName = "";
        var labelNodeName = "";
        switch (result.type) {
            case "report":
                nodeName = "div";
                labelNodeName = "h1";
                break;
            case "testcase":
                nodeName = "ol";
                labelNodeName = "h2";
                break;
            case "test":
                nodeName = "li";
                labelNodeName = "h3";
                break;
        }
        report = printTabs(depth) + "<" + nodeName + ">";	
        if (result.name) {
            if (result.type === "report" && currentTestFileName !== "") report += "<" + labelNodeName + '><a href="' + currentUrl + '">Test ' + currentTestFileName + " (" + currentSideName + "-Side)</a></" + labelNodeName + ">";
            else if (result.type !== "report") {
                if (result.name === "_name") report += "<" + labelNodeName + '><a name="name" href="' + currentUrl + '#name">name</a></' + labelNodeName + ">";
                else report += "<" + labelNodeName + '><a name="' + escapeXml(result.name) + '" href="' + currentUrl + '#' + escapeXml(result.name) + '">' + escapeXml(result.name) + "</a></" + labelNodeName + ">";
            }
        }				
        if (result.type === "test") {	
            if (result.result === "pass") report += ' <span style="color: green;">PASSED</span>';
            else if (result.result === "ignore") report += ' <span style="color: orange;">SKIPPED</span>';
            else report += ' <span style="color: red;">FAILED</span>';	
            var offset = 0;					
            if (result.result === "ignore") report += printTabs(depth + 1) + '<blockquote><pre style="color: grey;">' + escapeXml(result.message) + "</pre></blockquote>\n";
            else if (result.result !== "pass") report += printTabs(depth + 1) + '<blockquote><pre style="color: maroon;">' + escapeXml(result.message) + "</pre></blockquote>\n";
        }
        else {		
            report += ' <span style="color: green;">Passed: ' + result.passed + '</span> <span style="color: red;">Failed: ' + result.failed + '</span> <span style="color: gray;">Skiped: ' + result.ignored + '</span> <strong>Total: ' + result.total + "</strong>\n";
            for (var i in result) if (typeof result[i] === "object") report += formatHtmlReport(result[i], depth + 1);			
            if (depth === 0) report += " <strong>Success: " + Math.round(result.passed / result.total * 100) + "%</strong>"; 
        }									
        report += printTabs(depth) + "</" + nodeName + ">\n";
    }
    
    return report;
}

function doSendReport (report, url, location) {
    var req = new XMLHttpRequest();
    var payload = "name=" + encodeURIComponent(currentTestName);
    if (typeof location !== "undefined") payload += "&location=" + encodeURIComponent(location);
    payload += "&report=";
    if (typeof report === "string") payload += encodeURIComponent(report);
    else payload += encodeURIComponent(JSON.stringify(report));
    req.open("POST", url, false);
    req.setRequestHeader("Content-type", "text/plain; charset=utf-8");
    // FIXME ACI0070714 req.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    req.setRequestHeader("Content-length", payload.length);
    req.setRequestHeader("Expect", "");
    req.setRequestHeader("Connection", "close");
    req.send(payload);
    return this;
}

function escapeXml (string) {
	return string.replace(/[<>"'&]/g, function(value){
		switch(value){
			case "<":   return "&lt;";
			case ">":   return "&gt;";
			case "\"":  return "&quot;";
			case "'":   return "&apos;";
			case "&":   return "&amp;";
		}
	});
}

function printTabs (count) {
	var tabs = "";		
	for (var i = 0; i < count; i++) tabs += "    ";
	return tabs;
}