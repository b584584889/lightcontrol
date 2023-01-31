<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
  <link rel="shortcut icon" sizes="16x16 32x32 48x48 64x64" href="/favicon.ico">
  <link href="/css/vils.min.css" rel="stylesheet">

  <meta name="description" content="SMIMS VILS IoT">
  <meta name="author" content="SMIMS">
  <style>
  #nodeinfoft {
      position: absolute;
      top: 2px;
      width: 50%;
      text-align: left;
      z-index: 100;
      display:block;
  }

      .example-modal .modal {
      position: relative;
      top: auto;
      bottom: auto;
      right: auto;
      left: auto;
      display: block;
      z-index: 1;
    }

    .example-modal .modal {
      background: transparent !important;
    }

    /* 關閉背景黑色 */
    .modal-backdrop {
        /* bug fix - no overlay */
        /* display: none; */
        background: rgba(0,0,0,0);
    }

    /* 關閉背景黑色 */
    .modal {
        background: rgba(0,0,0,0.05);
    }

    .disabled {
        pointer-events: none;
        cursor: default;
    }

    .sidebar {
        max-height: calc(100vh - 8rem);
        overflow-y: auto;
    }

  </style>

  <title><?php echo $page_tilte ?></title>
</head>
