// Created by iWeb 2.0.4 local-build-20090628

function createMediaStream_id2()
{return IWCreatePhotocast("http://the-kenny.ath.cx/%7EMoritz/netwars/Netwars/Screenshots_files/rss.xml",true,true);}
function initializeMediaStream_id2()
{createMediaStream_id2().load('http://the-kenny.ath.cx/%7EMoritz/netwars/Netwars',function(imageStream)
{var entryCount=imageStream.length;var headerView=widgets['widget1'];headerView.setPreferenceForKey(imageStream.length,'entryCount');NotificationCenter.postNotification(new IWNotification('SetPage','id2',{pageIndex:0}));});}
function layoutMediaGrid_id2(range)
{createMediaStream_id2().load('http://the-kenny.ath.cx/%7EMoritz/netwars/Netwars',function(imageStream)
{if(range==null)
{range=new IWRange(0,imageStream.length);}
IWLayoutPhotoGrid('id2',new IWPhotoGridLayout(4,new IWSize(140,140),new IWSize(140,32),new IWSize(172,187),27,27,0,new IWSize(12,12)),new IWPhotoFrame([IWCreateImage('Screenshots_files/Formal_inset_01.png'),IWCreateImage('Screenshots_files/Formal_inset_02.png'),IWCreateImage('Screenshots_files/Formal_inset_03.png'),IWCreateImage('Screenshots_files/Formal_inset_06.png'),IWCreateImage('Screenshots_files/Formal_inset_09.png'),IWCreateImage('Screenshots_files/Formal_inset_08.png'),IWCreateImage('Screenshots_files/Formal_inset_07.png'),IWCreateImage('Screenshots_files/Formal_inset_04.png')],null,0,0.450000,1.000000,1.000000,1.000000,1.000000,14.000000,14.000000,14.000000,14.000000,191.000000,262.000000,191.000000,262.000000,null,null,null,0.100000),imageStream,range,null,null,1.000000,{backgroundColor:'#000000',reflectionHeight:100,reflectionOffset:2,captionHeight:100,fullScreen:0,transitionIndex:2},'Media/slideshow.html','widget1','widget2','widget3')});}
function relayoutMediaGrid_id2(notification)
{var userInfo=notification.userInfo();var range=userInfo['range'];layoutMediaGrid_id2(range);}
function onStubPage()
{var args=getArgs();parent.IWMediaStreamPhotoPageSetMediaStream(createMediaStream_id2(),args.id);}
if(window.stubPage)
{onStubPage();}
setTransparentGifURL('Media/transparent.gif');function applyEffects()
{var registry=IWCreateEffectRegistry();registry.registerEffects({stroke_0:new IWEmptyStroke()});registry.applyEffects();}
function hostedOnDM()
{return false;}
function onPageLoad()
{IWRegisterNamedImage('comment overlay','Media/Photo-Overlay-Comment.png')
IWRegisterNamedImage('movie overlay','Media/Photo-Overlay-Movie.png')
IWRegisterNamedImage('contribution overlay','Media/Photo-Overlay-Contribution.png')
loadMozillaCSS('Screenshots_files/ScreenshotsMoz.css')
adjustLineHeightIfTooBig('id1');adjustFontSizeIfTooBig('id1');NotificationCenter.addObserver(null,relayoutMediaGrid_id2,'RangeChanged','id2')
adjustLineHeightIfTooBig('id3');adjustFontSizeIfTooBig('id3');Widget.onload();fixAllIEPNGs('Media/transparent.gif');fixupIECSS3Opacity('id4');fixupIECSS3Opacity('id5');applyEffects()
initializeMediaStream_id2()}
function onPageUnload()
{Widget.onunload();}
