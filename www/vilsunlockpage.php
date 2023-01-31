<?php
    session_start();

    require_once("config.php");
    require_once("./php/nfclocator.php");

    $verify_error = '';
    $verify_pass = '';

    if ( !empty( $_POST ) )
    {
        if ( isset( $_POST['verifykey'] ) )
        {
            $verifykey = $_POST['verifykey'];
            $accountid = $_SESSION['accountid'];
            $userid = $_SESSION['userid'];

            $user_maingroupid = 1;
            $user_maingroups = explode(",", $_SESSION['maingroups']);
            if (count($user_maingroups) > 0)
                $user_maingroupid = $user_maingroups[0];

            list($verify_error, $verify_pass) = verifyNFCLocatorKey($verifykey, $accountid, $userid, $user_maingroupid);
            
            if( strlen($verify_pass) > 0 )
            {
                $_SESSION['type'] = 2;
            }
        }
    }

?>

<!DOCTYPE HTML>
<html lang="en">
<?php
    include ('vils_html_head.php');
?>

<body class="hold-transition register-page">
    <div class="register-box">
      <div class="register-logo">
        <b><h2><?php echo $unlockpage_tilte1;?></h2></b></a>
        <b><h2><?php echo $unlockpage_tilte2;?></h2></b></a>
      </div>

      <div class="register-box-body">

          <?php
              if (strlen($verify_error) > 0)
              {
                  echo '<div align="center"><h4><span class="label label-danger">'; echo $verify_error; echo'</span></h4></div>';
              }
              if (strlen($verify_pass) > 0)
              {
                  echo '<div align="center"><h4><span class="label label-success">'; echo $verify_pass; echo'</span></h4></div>';
              }
          ?>

          <form action="/vilsunlockpage.php" method="post">
              <div class="form-group has-feedback">
                  <input type="text" name="verifykey" class="form-control" placeholder=<?php echo $unlockpage_verifykey;?> value="" required>
                  <span class="glyphicon glyphicon-barcode form-control-feedback"></span>
              </div>

              <div class="row">
                <div class="col-xs-2">
                    <span class='logo-mini'>
                        <div class='image'>
                          <img src='/images/VILS_logo_s.jpg' class='img-circle' alt='VILS Logo' id='logo'>
                        </div>
                    </span>
                </div>
                <!-- /.col -->
                <div class="col-xs-3">
                    <button type="button" class="btn btn-success btn-block btn-flat pull-right"><?php echo $unlockpage_buy;?></button>
                </div>
                <div class="col-xs-2">
                </div>
                <!-- /.col -->
                <div class="col-xs-5">
                  <button type="submit" class="btn btn-primary btn-block btn-flat"><?php echo$unlockpage_registerdevice;?></button>
                </div>
                <!-- /.col -->
              </div>

        </form>


      </div>
      <!-- /.login-box-body -->
     </div>
     <!-- /.login-box -->

     <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
     <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

</body>

</html>
