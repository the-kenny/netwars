//
//  iWeb - Paste.js
//  Copyright (c) 2007 Apple Inc. All rights reserved.
//

function Paste(instanceID)
{if(instanceID)
{Widget.apply(this,arguments);}}
Paste.prototype=new Widget();Paste.prototype.constructor=Paste;Paste.prototype.widgetIdentifier="com-apple-iweb-widget-HTMLRegion";Paste.prototype.onload=function()
{if(!this.runningInApp)
this.setUpSubDocumentOnLoad();}
Paste.prototype.setUpSubDocumentOnLoad=function()
{var self=this;var oIFrame=this.getElementById("frame");setTimeout(function(){self.loadedSubDocument()},250);}
Paste.prototype.loadedSubDocument=function()
{var oIFrame=this.getElementById("frame");var oSubDocument=oIFrame.contentWindow||oIFrame.contentDocument;if(oSubDocument.document)
{oSubDocument=oSubDocument.document;}
if(oSubDocument.body)
{this.fixTargetOnElements(oSubDocument,"a");this.fixTargetOnElements(oSubDocument,"form");}
else
{var self=this;setTimeout(function(){self.loadedSubDocument()},250);}}
Paste.prototype.fixTargetOnElements=function(doc,tagName)
{var elements=doc.getElementsByTagName(tagName);for(var i=0;i<elements.length;i++)
{var target=elements[i].target;if(target===undefined||target=="")
elements[i].target="_top";}}
Paste.prototype.changedPreferenceForKey=function(key)
{if(key=="x-html-markup")
{this.div().innerHTML=this.div().innerHTML;this.preferences.postNotification("BLWidgetShouldStartAutoSizingNotification",1);}}
