package com.wakanda.qa.http.test.entitiy;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.util.Map;
import java.util.Map.Entry;

import junitparams.JUnitParamsRunner;
import junitparams.Parameters;

import org.apache.commons.lang.RandomStringUtils;
import org.apache.http.Header;
import org.apache.http.HttpEntity;
import org.apache.http.HttpHeaders;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.ByteArrayEntity;
import org.apache.http.entity.mime.FormBodyPart;
import org.apache.http.entity.mime.MultipartEntity;
import org.apache.http.entity.mime.content.ByteArrayBody;
import org.apache.http.entity.mime.content.StringBody;
import org.apache.http.protocol.HTTP;
import org.apache.http.util.EntityUtils;
import org.apache.log4j.Logger;
import org.hamcrest.core.Is;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Assume;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;

import com.wakanda.qa.http.CharSetUtil;
import com.wakanda.qa.http.MediaTypeUtil;
import com.wakanda.qa.http.Resources;
import com.wakanda.qa.http.test.extend.AbstractHttpTestCase;

/**
 * @author Ouissam
 * 
 *         This class manages all test cases related with media types.
 */
@RunWith(JUnitParamsRunner.class)
public class MediaTypeTest extends AbstractHttpTestCase {

	private static Logger logger = Logger.getLogger(MediaTypeTest.class);
	
	@BeforeClass
	public static void beforeClass() throws Exception{
		// create all resources file.*
		logger.debug("Setting media types resources...");
		MediaTypeUtil.setMediaTypeResources();
	}
	
	@AfterClass
	public static void afterClass() throws Exception{
		// delete all resources file.*
		logger.debug("Deleting media types resources...");
		MediaTypeUtil.removeMediaTypeResources();
	}
	
	/**
	 * <b>Implements:</b> MediaTypes01
	 * <p/>
	 * Check that server response contain a Content-Type header field.
	 * <p/>
	 * <b>Reference:</b> SPEC692 (RFC2616) 7.2.1
	 * 
	 * @throws Exception
	 */
	@Test
	public void testThatServerResponseContainsContentTypeHeaderField()
			throws Exception {
		HttpResponse response = executeDefaultRequest();
		assertEqualsStatusCode(HttpStatus.SC_OK, response);
		HttpEntity entity = response.getEntity();
		assertNotNull("Response has no content", entity);
		Header ctHeader = entity.getContentType();
		assertNotNull("Content-Type header field is missing", ctHeader);
	}

	/**
	 * <b>Implements:</b> MediaTypes02
	 * <p/>
	 * Check that the media type values matches the pattern media-type = type
	 * ""/"" subtype *( "";"" parameter ) type = token subtype = token
	 * <p/>
	 * <b>Reference:</b> SPEC689 (RFC2616) 3.7
	 * 
	 * @throws Exception
	 */
	@Test
	public void testThatServerReturnsCorrectContentTypeFormat()
			throws Exception {
		String regex = "([a-zA-Z0-9]+)/([a-zA-Z0-9]+)(;([a-zA-Z0-9]+)=([a-zA-Z0-9]+))*";
		HttpResponse response = executeDefaultRequest();
		String contentType = response.getEntity().getContentType().getValue();
		assertTrue("Content-Type header format is incorrect",
				contentType.matches(regex));
	}

	/**
	 * <b>Implements:</b> MediaTypes03
	 * <p/>
	 * Check that server returns the correct media type value for a resource
	 * extension.
	 * <p/>
	 * <b>Reference:</b> SPEC689 (RFC2616) 3.7 & Functional Specification
	 * 
	 * @throws Exception
	 */
	@Test
	@Parameters(method = "parameters")
	public void testThatServerReconizeMediaTypeFromFilesExtensions(
			String extension, String expectedMimeType) throws Exception {
		// send requests for extensions
		HttpGet request = new HttpGet("/mediaType/file." + extension);

		// Sending request for current extension
		HttpResponse response = executeRequest(request);
		assertEqualsStatusCode(HttpStatus.SC_OK, response);

		// check the returned content-type
		String actualContentType = EntityUtils.getContentMimeType(response
				.getEntity());
		assertEquals("Wrong media type for [" + extension + "]",
				expectedMimeType, actualContentType);
		EntityUtils.consume(response.getEntity());

	}

	@SuppressWarnings("unused")
	private Object[] parameters() throws Exception {
		Map<String, String> map = MediaTypeUtil.getMediaTypeMap();
		int lenght = map.size();
		Object[] tab = new Object[lenght];
		int i = 0;
		for (Entry<String, String> entry : map.entrySet()) {
			String extension = entry.getKey();
			String expectedContentType = entry.getValue();
			tab[i++] = new String[] { extension, expectedContentType };
		}
		return tab;
	}

	/**
	 * <b>Implements:</b> MediaTypes04
	 * <p/>
	 * Check that when the media Type is not known, Server should send
	 * application/octet-stream as Content-type header
	 * <p/>
	 * <b>Reference:</b> Functional specification 2 / Lot 1
	 * 
	 * @throws Exception
	 */
	@Test
	public void testThatServerRespondsWithApplicationOctetStreamWhenMedioTypeIsUnknowen()
			throws Exception {
		String expected = "application/octet-stream";
		String filename = "file.unknown";
		// create a file with unknown extension
		String mediaTypeFolder = Resources.getDefaultProjectWebFolderPath()
				+ "/mediaType";
		File mediaTypeDir = new File(mediaTypeFolder);
		if (!mediaTypeDir.exists())
			mediaTypeDir.mkdir();
		File file = new File(mediaTypeFolder + "/" + filename);
		if (!file.exists()) {
			file.createNewFile();
		}
		// System.out.println("File path :" + file.getPath());

		// request the file
		HttpGet request = new HttpGet("/mediaType/" + filename);

		HttpResponse response = executeRequest(request);

		// check the returned content-type
		String actual = EntityUtils.getContentMimeType(response.getEntity());

		assertEquals("Wrong media type", expected, actual);
	}

	/**
	 * <b>Implements:</b> MediaTypes05
	 * <p/>
	 * Check that the type, sub-type attribute names are case- insensitive
	 * <p/>
	 * <b>Reference:</b> SPEC689 (RFC2616) 3.7
	 * 
	 * @throws Exception
	 */
	@Test
	public void testThatMediaTypesTokensAreCaseInsensitive() throws Exception {
		// create a file with xml extension
		String filename = "file.xml";
		File file = new File(Resources.getMediaTypeFolder() + "/" + filename);
		if (!file.exists()) {
			file.createNewFile();
		}
		HttpGet request = new HttpGet("/mediaType/" + filename);
		
		// assume that server replay with 406 when media type is not acceptable
		request.addHeader(HttpHeaders.ACCEPT, "text/html");
		HttpResponse response = executeRequest(request);
		int actual = response.getStatusLine().getStatusCode();
		Assume.assumeThat(actual, Is.is(HttpStatus.SC_NOT_ACCEPTABLE));
		
		// add Accept header with the upper case value of the media type
		String expectedMimeType = "TExT/XmL";
		request.addHeader(HttpHeaders.ACCEPT, expectedMimeType);

		// Sending request for current extension
		response = executeRequest(request);
		assertEqualsStatusCode(HttpStatus.SC_OK, response);

		// check the returned content-type
		String actualContentType = EntityUtils.getContentMimeType(response
				.getEntity());
		assertEquals("["+ expectedMimeType + "] Media type should be case-insensitive", expectedMimeType,
				actualContentType);

	}

	/**
	 * <b>Implements:</b> MediaTypes06
	 * <p/>
	 * Check that server is able to parse multipart types.
	 * <p/>
	 * <b>Reference:</b> SPEC689 (RFC2616) 3.7.2
	 * 
	 * @throws Exception
	 */
	@Test
	public void testThatServerHandlesMultipartContent() throws Exception {
		// Build the multipost request
		HttpPost httppost = new HttpPost("/checkMultipart");

		// build the file part
		byte[] content0 = RandomStringUtils.random(256).getBytes();
		String fileName0 = "toCheckMultipart.tmp";
		ByteArrayBody contentBody0 = new ByteArrayBody(content0, fileName0);
		FormBodyPart part0 = new FormBodyPart("part0", contentBody0);

		// build the string part
		String content1 = RandomStringUtils.randomAscii(256);
		StringBody contentBody1 = new StringBody(content1);
		FormBodyPart part1 = new FormBodyPart("part1", contentBody1);

		// generate boundary
		String boundary = MediaTypeUtil.generateBoundary();

		// create request entity
		MultipartEntity reqEntity = new MultipartEntity(null, boundary, null);
		reqEntity.addPart(part0);
		reqEntity.addPart(part1);
		httppost.setEntity(reqEntity);

		// ByteArrayOutputStream out = new ByteArrayOutputStream();
		// reqEntity.writeTo(out);
		// logger.debug(out.toString());

		// execute request then parse the response
		HttpResponse response = executeRequest(httppost);
		String respCnt = EntityUtils.toString(response.getEntity());

		// logger.debug(respCnt);

		// check status code
		assertEqualsStatusCode(HttpStatus.SC_OK, response);

		// check content attributes
		String[] respCntPrts = respCnt.split("\n");
		// file part
		assertEquals("Wrong parts count", "2", respCntPrts[0]);
		assertEquals("Wrong parts content-type", "multipart/form-data",
				respCntPrts[1]);
		assertEquals("Wrong boundary", boundary, respCntPrts[2]);
		assertEquals("Wrong part name", part0.getName(), respCntPrts[3]);
		String expectedFileName = part0.getBody().getFilename();
		assertEquals("Wrong file name", expectedFileName, respCntPrts[4]);
		assertEquals("Wrong part content-type", part0.getBody().getMimeType(),
				respCntPrts[5]);
		assertEquals("Wrong part size", part0.getBody().getContentLength(),
				Long.parseLong(respCntPrts[6]));
		byte[] actuals = Resources.readFileAsBytes(Resources
				.getMultipartFolder() + "/" + expectedFileName);
		Assert.assertArrayEquals("Wrong part content", content0, actuals);

		// string part
		assertEquals("Wrong part name", part1.getName(), respCntPrts[7]);
		assertEquals("Wrong file name",
				part1.getBody().getFilename() == null ? "" : part1.getBody()
						.getFilename(), respCntPrts[8]);
		assertEquals("Wrong part content-type", part1.getBody().getMimeType(),
				respCntPrts[9]);
		assertEquals("Wrong part size", part1.getBody().getContentLength(),
				Long.parseLong(respCntPrts[10]));
		assertEquals("Wrong part content",
				Resources.convertStreamToString(((StringBody) part1.getBody())
						.getReader()), respCntPrts[11]);
	}

	/**
	 * <b>Implements:</b> MediaTypes07
	 * <p/>
	 * Check that when an Accept header field is present, and if the server
	 * cannot send a response which is acceptable according to the combined
	 * Accept field value, then the server SHOULD send a 406 (Not Acceptable)
	 * response.
	 * <p/>
	 * <b>Reference:</b> SPEC698 (RFC2616) 14.1
	 * 
	 * @throws Exception
	 */
	@Test
	public void testThatServerReplaysWith406WhenMediaTypeIsNotAcceptable()
			throws Exception {
		// create a file with xml extension
		String filename = "file.xml";
		File file = new File(Resources.getMediaTypeFolder() + "/" + filename);
		if (!file.exists()) {
			file.createNewFile();
		}
		// request that accepts only text/html type
		HttpGet request = new HttpGet("/mediaType/" + filename);
		request.addHeader(HttpHeaders.ACCEPT, "text/html");
		HttpResponse response = executeRequest(request);
		int actual = response.getStatusLine().getStatusCode();
		assertEquals(
				"Server should reply with 406 when media type is not acceptable",
				HttpStatus.SC_NOT_ACCEPTABLE, actual);
	}

	/**
	 * <b>Implements:</b> MediaTypes08
	 * <p/>
	 * Check that when no explicit charset parameter is provided by the sender,
	 * media subtypes of the "text" type are defined to have a default charset
	 * value of "ISO-8859-1" when received via HTTP.
	 * <p/>
	 * <b>Reference:</b> SPEC698 (RFC2616) 3.7.1
	 * 
	 * @throws Exception
	 */
	@Test
	public void testThat_ISO_8859_1_IsDefaultForTextType() throws Exception {
		String charset = HTTP.ISO_8859_1;
		String mt = HTTP.PLAIN_TEXT_TYPE;
		// String ct = mt + HTTP.CHARSET_PARAM + charset;

		// get the unicode character set of the specified encoding.
		char[] chars = CharSetUtil.getSupportedCharacrters(charset, false);

		// create a random string from that set
		String reqSContent = RandomStringUtils.random(10, 0, chars.length,
				true, false, chars);

		// convert that string into array of bytes
		byte[] reqBConvertedContent = reqSContent.getBytes(charset);

		// create the request entity
		ByteArrayEntity reqEntity = new ByteArrayEntity(reqBConvertedContent);
		reqEntity.setContentType(mt);

		// long reqCLength = reqEntity.getContentLength();
		// logger.debug("Request content-length: " + reqCLength);

		// request
		HttpPost request = new HttpPost("/checkPostMethod/");
		// StringEntity reqEntity0 = new StringEntity("Hello");
		request.setEntity(reqEntity);

		// response
		HttpResponse response = executeRequest(request);
		assertEqualsStatusCode(HttpStatus.SC_OK, response);
		HttpEntity resEntity = response.getEntity();
		assertNotNull(resEntity);

		// is content-length > 0
		long resCLength = resEntity.getContentLength();
		// logger.debug("Response content-length: " + resCLength);
		assertTrue("Response must be > 0", resCLength > 0);

		// get the response content as string using response charset parameter
		String resSContent = EntityUtils.toString(resEntity);

		// if server decodes request body using ISO_8859_1 charset, then its
		// response content after being decoded would equal the request content.
		assertEquals("Wrong response content", reqSContent, resSContent);

	}

	/**
	 * <b>Implements:</b> MediaTypes09
	 * <p/>
	 * Check that server responds with 415 when the media type of the request
	 * entity is in a format not supported.
	 * <p/>
	 * <b>Reference:</b> SPEC695 (RFC2616) 10.4.16
	 * 
	 * @throws Exception
	 */
	@Test
	public void testThatServerReplaysWith415WhenMediaTypeOfRequestEntityIsNotSupported()
			throws Exception {
		// entity
		String mt = "whatever/whatever";
		ByteArrayEntity reqEntity = new ByteArrayEntity("whatever".getBytes());
		reqEntity.setContentType(mt);

		// request
		HttpPost request = new HttpPost("/checkPostMethod/");
		request.setEntity(reqEntity);

		// response
		HttpResponse response = executeRequest(request);
		assertEqualsStatusCode(HttpStatus.SC_UNSUPPORTED_MEDIA_TYPE, response);
	}

	/**
	 * <b>Implements:</b> MediaTypes10
	 * <p/>
	 * Check that server responds with 406 when the media type contains space between the type and subtype.
	 * <p/>
	 * <b>Reference:</b> SPEC689 (RFC2616) 3.7
	 * 
	 * @throws Exception
	 */
	@Test
	public void testThatServerReplaysWith406WhenMediaTypeContainsSpaceBetweenTypeAndSubtype()
			throws Exception {
		// create a file with xml extension
		String filename = "file.html";
		File file = new File(Resources.getMediaTypeFolder() + "/" + filename);
		if (!file.exists()) {
			file.createNewFile();
		}
		// request that accepts only text/html type
		HttpGet request = new HttpGet("/mediaType/" + filename);
		request.addHeader(HttpHeaders.ACCEPT, "text / html");
		HttpResponse response = executeRequest(request);
		assertEqualsStatusCode(HttpStatus.SC_NOT_ACCEPTABLE, response);
	}
}
