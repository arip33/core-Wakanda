template = '\
<div id="__application.id__" data-application-number="__application.number__" class="application">\
	<div class="head">\
		<div class="border left"></div>\
		<div class="border right"></div>\
		<div class="content">\
			<h2>__application.name__</h2>\
			</div>\
	</div>\
	<div class="toolbar">\
		<div class="border left"></div>\
		<div class="border right"></div>\
		<div class="content">\
			<ul id="application-0-toolbar">\
				<li class="application-goto" title="Go to application"><button>Go to application</button></li>\
				<li class="separator"></li>\
				<li class="catalog-goto" title="Go to model"><button>Go to model</button></li>\
				<li class="separator"></li>\
				<li class="database-flush" title="Flush database"><button>Flush database</button></li>\
				<li class="separator"></li>\
				<li class="lock" title="Lock / Unlock application"><button>Lock / Unlock application</button></li>\
				</ul>\
		</div>\
		<hr />\
	</div>\
	<div class="body">\
		<div class="border right"></div>\
		<div class="border bottom"></div>\
		<div class="border left"></div>\
		<div class="corner bottom right"></div>\
		<div class="corner bottom left"></div>\
		<div class="content">\
			<form onsubmit="return false;">\
				<table>\
					<col />\
					<col />\
					<tr>\
						<td>\
							<table class="settings">\
								<col />\
								<col />\
								<tr>\
									<td align="right">Http :</td><td><div class="http status __(application.http.enabled) ? "started" : " "__"></div><input type="checkbox" name="http" data-type="server" __(application.http.enabled) ? "checked" : " "__ /></td>\
								</tr>\
								<tr>\
									<td align="right">IP :</td><td><input type="text" name="ip" value="__application.http.ip__" readonly="readonly" /></td>\
								</tr>\
								<tr>\
									<td align="right">Hostname :</td><td><input type="text" name="hostname" value="__application.http.hostName__" readonly="readonly" /></td>\
								</tr>\
								<tr>\
									<td align="right">HTTP Port :</td><td><input type="text" name="http-port" value="__application.http.port__" readonly="readonly" /></td>\
								</tr>\
							</table>\
						</td>\
						<td>\
							<table class="services">\
								<col />\
								<col />\
								<col />\
								<tr>\
									<td align="right">WebApp :</td><td><div class="webApp status __(application.webApp.enabled) ? "started" : " "__"></div><input type="checkbox" name="webApp" data-type="service" __(application.webApp.enabled) ? "checked" : " "__ /></td>\
								</tr>\
								<tr>\
									<td align="right">File Service :</td><td><div class="file status __(application.fileService.enabled) ? "started" : " "__"></div><input type="checkbox" name="file" data-type="service" __(application.fileService.enabled) ? "checked" : " "__ /></td>\
								</tr>\
								<tr>\
									<td align="right">Data Service :</td><td><div class="data status __(application.dataService.enabled) ? "started" : " "__"></div><input type="checkbox" name="data" data-type="service" __(application.dataService.enabled) ? "checked" : " "__ /></td>\
								</tr>\
								<tr>\
									<td align="right">RPC :</td><td><div class="rpc status __(application.rpcService.enabled) ? "started" : " "__"></div><input type="checkbox" name="rpc" data-type="service" __(application.rpcService.enabled) ? "checked" : " "__ /></td>\
								</tr>\
							</table>\
						</td>\
					</tr>\
				</table>\
			</form>\
		</div>\
	</div>\
</div>';