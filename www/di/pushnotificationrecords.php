<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vi_html_head.php');
    ?>

      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilspushnotification_records;?></h3>

          <span>&nbsp;&nbsp;</span>
          <div class="btn-group">
            <button type="button" class="btn btn-default" id="current_type_title"></button>
            <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
              <span class="caret"></span>
              <span class="sr-only">Toggle Dropdown</span>
            </button>
            <ul class="dropdown-menu" id="dropdown_menu_type" role="menu">
            </ul>
          </div>

          <span>&nbsp;&nbsp;</span>
          <div class="btn-group">
            <button type="button" class="btn btn-default" id="current_theme_title"></button>
            <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
              <span class="caret"></span>
              <span class="sr-only">Toggle Dropdown</span>
            </button>
            <ul class="dropdown-menu" id="dropdown_menu_theme" role="menu">
            </ul>
          </div>

          <div class="box-tools pull-right">
              <ul class="pagination pagination-sm no-margin">
                <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                <li><a href="#" id="id_current_page_num">1</a></li>
                <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
              </ul>
              <ul class="pagination pagination-sm no-margin pull-right">
                <li>
                  <button type="button" class="btn btn-box-tool" onclick="open_pushnotificationrecords_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                </li>
              </ul>
          </div>

            <div class="btn-group">
                <input type="text" id="search_notify_target_text" placeholder="??????????????????">
            </div>
            <div class="btn-group">
                <input type="text" id="search_notify_date_text" placeholder="??????:YYYY-MM-DD">
            </div>

        </div>
        <!-- /.box-header -->
        <div class="box-body">

            <div style="height:650px;overflow:scroll;">
                <table class="table table-bordered">
                    <thead>
                        <tr style="background-color:rgb(236, 240, 245);">
                            <th style="width: 10px">#</th>
                            <th style="width: 130px">????????????</th>
                            <th style="width: 80px">????????????</th>
                            <th style="width: 80px">????????????</th>
                            <th style="width: 80px">?????????</th>
                            <th>Title</th>
                            <th>????????????</th>
                            <th style="width: 100px"??????</th>
                            <th style="width: 50px">??????</th>
                        </tr>
                    </thead>
                    <tbody id="table_content">
                    </tbody>
                </table>
            </div>


          <div class="nav-tabs-custom">
            <ul class="nav nav-tabs" id="nav_tabs">
            </ul>
            <div class="tab-content" id="tab_content">
            </div>
          </div>

        </div>
        <!-- /.box-body -->
      </div>
      <!-- /.box -->

      <div style="text-align:center">
          <button class="btn btn-default btn-flat" onclick="location.href='/di'" style="display:block;display:table;margin:0 auto;"><i class="fa fa-reply"></i>??????</button>
      </div>


    <script>
        var project_id = 1;
        var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_userid = "<?php echo $_SESSION["userid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/pushnotificationrecords.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/pushnotificationrecords_src/00-pushnotificationrecords.js"></script> -->
</html>
