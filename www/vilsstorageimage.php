<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
    <link href="/css/vils.min.css" rel="stylesheet">
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - Image Storage</title>
  </head>


    <div class="modal fade" id="modal-image_detail">
      <div class="modal-dialog">
        <div class="modal-content">
          <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-label="Close">
              <span aria-hidden="true">&times;</span></button>
            <h4 class="modal-title">相片詳細資訊</h4>
          </div>
          <div class="modal-body">
            <div id='image_detail_name'></div>
            <div id='image_detail_description'></div>
            <div id='image_detail_filename'></div>
            <div id='image_detail_filetype'></div>
            <div id='image_detail_subgroups'></div>
            <div id='image_detail_account'></div>
            <!-- <div id='file_detail_updatetime'></div> -->
            <div id='image_detail_uploadtime'></div>
          </div>
          <div class="modal-footer">
            <button type="button" class="btn btn-danger pull-left" id="delete_button" style="display:none;">Delete</button>
            <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
          </div>
        </div>
        <!-- /.modal-content -->
      </div>
      <!-- /.modal-dialog -->
    </div>
    <!-- /.modal -->

  <div class="row">
    <div class="col-xs-12">
        <div class="box">
          <div class="box-header">
            <h3 class="box-title"><?php echo $left_side_bar_filestorage_image; ?></h3>

            <?php
            if ($_SESSION["groupid"] != 3 )
            {
                echo '<a class="btn btn-success btn-xs" onclick="create_new_filestorage_image();">'.$filestorage_creatnewimage.'</a>';
            }
            ?>

            <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" onclick="open_storageimage_page();return false;"><i class="fa fa-external-link"></i>
                </button>
            </div>
          </div>
          <!-- /.box-header -->
          <div class="box-body" style="margin:10px">


            <table id="table_storage_image" class="table table-bordered table-striped" style="display:none;">
              <thead>
              <tr style="background-color:#cce6ff;">
                <th>相片名稱</th>
                <th>相片說明</th>
                <th style="width: 300px">相片子集合權限</th>
                <th>相片</th>
                <th>上傳帳號</th>
                <!-- <th>最後編輯時間</th>
                <th>上傳時間</th> -->
                <th style="width: 100px">操作</th>
              </tr>
              </thead>
              <tbody id="tablebody_storage_image">

              </tbody>
              <tfoot>
              </tfoot>
            </table>

            <div class="row table-responsive" id="image_list_area">

            </div>

          </div>
          <!-- /.box-body -->
        </div>
        <!-- /.box -->

    </div>
    <!-- /.col -->
  </div>
  <!-- /.row -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
      var config_enable_edit = <?php echo $GL_CONFIG_EDIT_USERINFO ?>;
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storageimage.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/storageimage_src/00-jquery.dataTables.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storageimage_src/01-dataTables.bootstrap.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storageimage_src/02-uploadimage.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storageimage_src/03-storageimage.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
