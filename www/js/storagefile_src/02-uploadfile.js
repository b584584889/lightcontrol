/*
 * Author: ikki Chung
 * Date: 2020/07/20
 * Description:
 *      SMIMS VILS RTLS Solutions event trigger
 **/

//------------------------------------------------------------------------------------------------------------------------------------

window.BlobBuilder = window.MozBlobBuilder || window.WebKitBlobBuilder || window.BlobBuilder;

var blob;
var start;
var end;
var part;
var uploaded;
var SIZE;
var BYTES_PER_CHUNK;
var xhr;
var chunk;
var UploadFileFinishedCallback = null;
var uid = '';

function sendUploadFileRequest(callback)
{
    blob = document.getElementById('fileUpload').files[0];

    if ( blob == undefined )
    {
        alert("讀取檔案失敗!");
        return null;
    }

    BYTES_PER_CHUNK = 1048576; // 1MB chunk sizes.
    SIZE = blob.size;
    start = 0;
    part = 0;
    end = BYTES_PER_CHUNK;
    uploaded = 0;

    UploadFileFinishedCallback = callback;
    uid = Date.now();

    chunk = blob.slice(start, end);
    uploadFileFile(chunk, part);
    start = end;
    end = start + BYTES_PER_CHUNK;
    part = part + 1;
    uploaded = 0;
    return uid;
};
//------------------------------------------------------------------------------------------------------------------------------------

function fileSelected()
{
    var file = document.getElementById('fileUpload').files[0];
    if (file)
    {
        var fileSize = 0;
        if (file.size > 1024 * 1024)
            fileSize = (Math.round(file.size * 100 / (1024 * 1024)) / 100).toString() + 'MB';
        else
            fileSize = (Math.round(file.size * 100 / 1024) / 100).toString() + 'KB';

        //document.getElementById('fileName').innerHTML = 'Name: ' + file.name;

        var filename = file.name.split(".");
        var id_new_file_name = document.getElementById('id_new_file_name');
        if( id_new_file_name != undefined )
            id_new_file_name.value = filename[0];
        var id_new_file_description = document.getElementById('id_new_file_description');
        if( id_new_file_description != undefined )
            id_new_file_description.value = filename[0];

        document.getElementById('fileFileSize').innerHTML = 'Size: ' + fileSize;
        document.getElementById('fileFileType').innerHTML = 'Type: ' + file.type;
    }
};
//------------------------------------------------------------------------------------------------------------------------------------

function uploadFileFile(blobFile, part)
{
    var file = document.getElementById('fileUpload').files[0];
    var fd = new FormData();
    fd.append("fileUpload", blobFile);
    fd.append("file", file.name);
    fd.append("uid", uid);
    fd.append("num", parseInt(part));

    var xhr = new XMLHttpRequest();
    xhr.upload.addEventListener("progress", uploadFileProgress, false);
    xhr.addEventListener("load", uploadFileComplete, false);
    xhr.addEventListener("error", uploadFileFailed, false);
    xhr.addEventListener("abort", uploadFileCanceled, false);

    //xhr.open("POST", "/php/uploadfile.php?fileUpload=1&file="+file.name+"&uid="+uid+"&num=" + parseInt(part) );
    xhr.open("POST", "/php/uploadfile.php" );
    xhr.onload = function(e) {
      //alert("loaded!");
      };

    xhr.setRequestHeader('Cache-Control','no-cache');
    xhr.send(fd);
    return;

};
//------------------------------------------------------------------------------------------------------------------------------------

function uploadFileProgress(evt)
{
    if (evt.lengthComputable)
    {
        //var percentComplete = Math.round(evt.loaded * 100 / evt.total);
        var percentComplete = Math.round((uploaded + evt.loaded) * 100 / SIZE);
        document.getElementById('fileProgressNumber').innerHTML = percentComplete.toString() + "%";
    }
    else
    {
        document.getElementById('fileProgressNumber').innerHTML = 'unable to compute';
    }
}
//------------------------------------------------------------------------------------------------------------------------------------

function uploadFileComplete(evt)
{
    // This event is raised when the server send back a response
    //alert(evt.target.responseText);

    if( start < SIZE )
    {
        chunk = blob.slice(start, end);

        uploadFileFile(chunk,part);

        start = end;
        end = start + BYTES_PER_CHUNK;
        part = part + 1;
        uploaded = uploaded + BYTES_PER_CHUNK;
    }
    else
    {
        if (typeof UploadFileFinishedCallback === "function")
        {
            var response = JSON.parse(evt.target.responseText);
            if ( response.FileSize == 0 )
                UploadFileFinishedCallback(null);
            else
                UploadFileFinishedCallback(uid);
        }
    }
};
//------------------------------------------------------------------------------------------------------------------------------------

function uploadFileFailed(evt)
{
    alert("There was an error attempting to upload the file.");
    uid = null;
};
//------------------------------------------------------------------------------------------------------------------------------------

function uploadFileCanceled(evt)
{
    xhr.abort();
    xhr = null;
    uid = null;
    alert("The upload has been canceled by the user or the browser dropped the connection.");
};
//------------------------------------------------------------------------------------------------------------------------------------
