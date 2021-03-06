package com.wakanda.qa.http.test.statuscodes;

import static org.junit.Assert.*;

import org.apache.http.HttpHeaders;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.EnglishReasonPhraseCatalog;
import org.junit.Test;

import com.wakanda.qa.http.test.extend.AbstractHttpTestCase;

/**
 * This class manages all test cases related with status codes that begin with
 * 4xx (Client Error)
 * 
 * @author Ouissam
 * 
 */
public class StatusCode4xxTest extends AbstractHttpTestCase {

	/**
	 * <b>Implements:</b> StatusCode400
	 * <p/>
	 * Check 400 Bad Request
	 * <p/>
	 * <b>Reference:</b> SPEC695 = RFC2616 10.4.1
	 * 
	 * @throws Exception
	 */
	@Test
	public void testStatusCode400BadRequest() throws Exception {
		int expected = HttpStatus.SC_BAD_REQUEST;
		String badRequest = "GET " + CRLF
							+ CRLF;
		HttpResponse response = executeRequestString(badRequest);
		assertEquals("Wrong status code", expected, response
				.getStatusLine().getStatusCode());

		assertEquals("Wrong reason phrase",
				EnglishReasonPhraseCatalog.INSTANCE.getReason(
						expected, null), response
						.getStatusLine().getReasonPhrase());
	}

	/**
	 * <b>Implements:</b> StatusCode404
	 * <p/>
	 * Check 404 Not Found
	 * <p/>
	 * <b>Reference:</b> SPEC695 = RFC2616 10.4.5
	 * 
	 * @throws Exception
	 */
	@Test
	public void testStatusCode404NotFound() throws Exception {
		int expected = HttpStatus.SC_NOT_FOUND;
		HttpGet httpget = new HttpGet("/NotFound.html");
		HttpResponse response = executeRequest(httpget);
		assertEquals("Wrong status code", expected, response.getStatusLine()
				.getStatusCode());

		assertEquals(
				"Wrong reason phrase",
				EnglishReasonPhraseCatalog.INSTANCE.getReason(expected, null),
				response.getStatusLine().getReasonPhrase());
	}

	/**
	 * <b>Implements:</b> StatusCode405
	 * <p/>
	 * Check 405 Method Not Allowed
	 * <p/>
	 * <b>Reference:</b> SPEC695 = RFC2616 10.4.6
	 * 
	 * @throws Exception
	 */
	@Test
	public void testStatusCode405NotAllowed() throws Exception {
		int expected = HttpStatus.SC_METHOD_NOT_ALLOWED;
		HttpPut request = new HttpPut(getDefaultUrl());
		HttpResponse response = executeRequest(request);
		assertEquals("Wrong status code", expected, response.getStatusLine()
				.getStatusCode());
		assertEquals(
				"Wrong reason phrase",
				EnglishReasonPhraseCatalog.INSTANCE.getReason(expected, null),
				response.getStatusLine().getReasonPhrase());
		assertEquals("Wrong Allow header values", "GET, HEAD, TRACE", response
				.getFirstHeader(HttpHeaders.ALLOW).getValue());
	}
	
	/**
	 * <b>Implements:</b> StatusCode401
	 * <p/>
	 * Check 401 Unauthorized
	 * <p/>
	 * <b>Reference:</b> SPEC695 = RFC2616 10.4.2
	 * 
	 * @throws Exception
	 */
	@Test
	public void testStatusCode401Unauthorized() throws Exception {
		int expected = HttpStatus.SC_UNAUTHORIZED;
		String url = "/authBasic/";
		HttpGet request = new HttpGet(url);
		HttpResponse response = executeRequest(request);
		// check status code
		assertEquals("Wrong status code", expected, response
				.getStatusLine().getStatusCode());

		assertEquals("Wrong reason phrase",
				EnglishReasonPhraseCatalog.INSTANCE.getReason(
						expected, null), response
						.getStatusLine().getReasonPhrase());
	}
	
	/**
	 * <b>Implements:</b> StatusCode406
	 * <p/>
	 * Check 406 Not Acceptable
	 * <p/>
	 * <b>Reference:</b> SPEC695 = RFC2616 10.4.7
	 * 
	 * @throws Exception
	 */
	@Test
	public void testStatusCode406NotAcceptable() throws Exception {
		int expected = HttpStatus.SC_NOT_ACCEPTABLE;
		HttpGet request = new HttpGet("/tocompress.html");
		request.addHeader(HttpHeaders.ACCEPT_ENCODING, "whatever");
		HttpResponse response = executeRequest(request);
		assertEquals("Wrong staus code",
				expected, response.getStatusLine()
						.getStatusCode());
		assertEquals("Wrong reason phrase",
				EnglishReasonPhraseCatalog.INSTANCE.getReason(
						expected, null), response
						.getStatusLine().getReasonPhrase());
	}
	
	/**
	 * <b>Implements:</b> StatusCode415
	 * <p/>
	 * Check 415 Unsupported Media Type
	 * <p/>
	 * <b>Reference:</b> SPEC695 = RFC2616 10.4.16 & SPEC698 = RFC2616 14.11
	 * 
	 * @throws Exception
	 */
	@Test
	public void testStatusCode415UnsupportedMediaType() throws Exception {
		int expected = HttpStatus.SC_UNSUPPORTED_MEDIA_TYPE;
		
		HttpPost request = new HttpPost("/checkPostMethod/");
		StringEntity entity = new StringEntity("whatever");
		entity.setContentEncoding("whatever");
		request.setEntity(entity);
		
		HttpResponse response = executeRequest(request);
		
		assertEquals("Wrong staus code",
				expected, response.getStatusLine()
						.getStatusCode());
		assertEquals("Wrong reason phrase",
				EnglishReasonPhraseCatalog.INSTANCE.getReason(
						expected, null), response
						.getStatusLine().getReasonPhrase());
	}

}
