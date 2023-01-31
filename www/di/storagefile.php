<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vi_html_head.php');
    ?>

    <div class="modal fade" id="modal-file_detail">
      <div class="modal-dialog">
        <div class="modal-content">
          <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal" aria-label="Close">
              <span aria-hidden="true">&times;</span></button>
            <h4 class="modal-title">檔案詳細資訊</h4>
          </div>
          <div class="modal-body">
            <div id='file_detail_name'></div>
            <div id='file_detail_description'></div>
            <div id='file_detail_filename'></div>
            <div id='file_detail_filetype'></div>
            <div id='file_detail_subgroups'></div>
            <div id='file_detail_account'></div>
            <!-- <div id='file_detail_updatetime'></div> -->
            <div id='file_detail_uploadtime'></div>
          </div>
          <div class="modal-footer">
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
            <h3 class="box-title"><?php echo $filestorage_file; ?></h3>

            <span>&nbsp;</span>

            <?php
            if ($_SESSION["groupid"] != 3 )
            {
                echo '<a class="btn btn-success btn-xs" onclick="create_new_filestorage_file();">'.$filestorage_creatnewfile.'</a>';
            }
            ?>

            <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" onclick="open_storagefile_page();return false;"><i class="fa fa-external-link"></i>
                </button>
            </div>
          </div>
          <!-- /.box-header -->
          <div class="box-body table-responsive">
            <table id="table_storage_file" class="table table-bordered table-striped" style="display:none;">
              <thead>
              <tr style="background-color:#cce6ff;">
                <th>檔案名稱</th>
                <th>檔案說明</th>
                <th>檔案子集合權限</th>
                <th>檔案</th>
                <th>上傳帳號</th>
                <!-- <th>最後編輯時間</th>
                <th>上傳時間</th> -->
                <th style="width: 100px">操作</th>
              </tr>
              </thead>
              <tbody id="tablebody_storage_file">

              </tbody>
              <tfoot>
              </tfoot>
            </table>

              <div class="row table-responsive" id="file_list_area">

          </div>
          <!-- /.box-body -->
        </div>
        <!-- /.box -->

            <div style="text-align:center">
                <button class="btn btn-default btn-flat" onclick="location.href='/di'" style="display:block;display:table;margin:0 auto;"><i class="fa fa-reply"></i>返回</button>
            </div>

    </div>
    <!-- /.col -->
  </div>
  <!-- /.row -->

    <script>
      var project_id = 1;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storagefile.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/storagefile_src/00-jquery.dataTables.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storagefile_src/01-dataTables.bootstrap.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storagefile_src/02-uploadfile.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storagefile_src/03-storagefile.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
