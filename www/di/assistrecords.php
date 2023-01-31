<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">

<?php
include ('vi_html_head.php');
?>


    <style type="text/css">
    .datepicker table tr td.active:active,
    .datepicker table tr td.active:hover:active,
    .datepicker table tr td.active.disabled:active,
    .datepicker table tr td.active.disabled:hover:active,
    .datepicker table tr td.active.active,
    .datepicker table tr td.active:hover.active,
    .datepicker table tr td.active.disabled.active,
    .datepicker table tr td.active.disabled:hover.active
    {
      background-image: linear-gradient(to bottom, #cc4400, #994411);
    }
    /* .datepicker table tr td.highlight:active,
    .datepicker table tr td.highlight:hover:active,
    .datepicker table tr td.highlight.disabled:active,
    .datepicker table tr td.highlight.disabled:hover:active,
    .datepicker table tr td.highlight.active,
    .datepicker table tr td.highlight:hover.active,
    .datepicker table tr td.highlight.disabled.active,
    .datepicker table tr td.highlight.disabled:hover.active
    {
        background-image: linear-gradient(to bottom, #cc4400, #994411);
    } */
    .datepicker table tr td.new { display: none; }
    .datepicker table tr td.old { visibility: hidden; }
    .datepicker {
        background-color: #9fc49f !important;
        color: #333333 !important;
    }

    </style>


      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $assistrecord_title;?></h3>

          <span>&nbsp;</span>
          <a class='btn btn-success btn-xs' onclick='apply_new_assist();'><?php echo $assist_applynew; ?></a>

          <div class="box-tools pull-right">
              <ul class="pagination pagination-sm no-margin">
                <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                <li><a href="#" id="id_current_page_num">1</a></li>
                <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
              </ul>
              <ul class="pagination pagination-sm no-margin pull-right">
                <li>
                  <button type="button" class="btn btn-box-tool" onclick="open_assistrecords_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                </li>
              </ul>
          </div>
        </div>
        <!-- /.box-header -->
        <div class="box-body table-responsive">

          <table class="table table-bordered" id="ASSIST_EVENT_TABLE">
            <thead>
              <tr style="background-color:rgb(236, 240, 245);">
                <th style="width: 30px">#</th>
                <th>申請時間</th>
                <th>事項名稱</th>
                <th>事項說明</th>
                <th>申請說明</th>
                <th>協助對象</th>
                <th>每日協助時間</th>
                <th style="width: 180px">協助日期</th>
                <th>附件照片</th>
                <th>附件檔案</th>
                <th>申請人姓名</th>
                <th>申請人關係</th>
                <th>狀態</th>
              </tr>
            </thead>
            <tbody id="ASSIST_EVENT_TABLE_BODY">

            </tbody>
          </table>

        </div>
        <!-- /.box-body -->
      </div>
      <!-- /.box -->

      <div style="text-align:center">
          <button class="btn btn-default btn-flat" onclick="location.href='/di'" style="display:block;display:table;margin:0 auto;"><i class="fa fa-reply"></i>返回</button>
      </div>

    <script>
        var project_id =  1;
        var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/assistrecords.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/assistrecords_src/00-bootstrap-timepicker.js"></script>
    <script src="/js/assistrecords_src/01-bootstrap-datepicker.js"></script>
    <script src="/js/locales/bootstrap-datepicker.zh-TW.min.js"></script>
    <script src="/js/storagefile_src/02-uploadfile.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storageimage_src/02-uploadimage.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/assistrecords_src/02-assistrecords.js"></script> -->
</html>
