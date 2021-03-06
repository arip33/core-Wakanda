/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.
 * See the License for the specific language governing rights and
 * limitations under the License.
 *
 * The Original Code is Bespin.
 *
 * The Initial Developer of the Original Code is Mozilla.
 * Portions created by the Initial Developer are Copyright (C) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Bespin Team (bespin@mozilla.com)
 *
 * ***** END LICENSE BLOCK ***** */

dojo.provide("bespin.page.dashboard.events");

if (!bespin.page.dashboard) bespin.page.dashboard = {};

// After a project is imported or created, do a list
bespin.subscribe("project:imported", function(event) {
    bespin.page.dashboard.refreshProjects(); // get projects
});

bespin.subscribe("project:set", function(event) {
    bespin.get('editSession').project = event.project; // set it in the session

    if (!event.fromDashboardItemSelected) {
        // selects the project in the tree and fire the itemselected event
        bespin.page.dashboard.tree.lists[0].selectItemByText(event.project);
        bespin.page.dashboard.tree.itemSelected({thComponent: bespin.page.dashboard.tree.lists[0], item: bespin.page.dashboard.tree.lists[0].selected});
    }
});

bespin.subscribe("project:create", function(event) {
    bespin.page.dashboard.refreshProjects(); // get projects
});

// JOE: I don't see how this could ever work. project:delete is a request for
// action rather than a happened action. It might fluke it if this is called
// after the other subscription. Based on this flaw and the fact that we're
// deleting the dashbaord anyway, I'm not going to fix this. This event will
// not be called.
bespin.subscribe("project:delete", function(event) {
    bespin.page.dashboard.refreshProjects(); // get projects
});

// ** {{{ Event: session:status }}} **
//
// Observe a request for session status
bespin.subscribe("session:status", function(event) {
    var editSession = bespin.get('editSession');
    var msg = 'Hey ' + editSession.username;

    if (editSession.project) {
        msg += ', you appear to be highlighting the project ' + editSession.project;
    } else {
        msg += ", you haven't select a project yet.";
    }

    bespin.get("commandLine").addOutput(msg);
});

// ** {{{ Event: editor:newfile }}} **
//
// Observe a request for a new file to be created
bespin.subscribe("editor:newfile", function(event) {
    var project = event.project || bespin.get('editSession').project;

    if (!project) {
        bespin.get("commandLine").addErrorOutput('The new file action requires a project');
        return;
    }

    var newfilename = event.newfilename || "new.txt";

    var opts = { newFile: true };
    if (event.content) opts.content = event.content;

    bespin.util.navigate.editor(project, newfilename, opts);
});

// ** {{{ Event: editor:openfile }}} **
//
// Cause the editor to launch
bespin.subscribe("editor:openfile", function(event) {
    bespin.util.navigate.editor(event.project, event.filename, {});
});
