/* Copyright (c) 2006-2008 MetaCarta, Inc., published under the Clear BSD
 * license.  See http://svn.openlayers.org/trunk/openlayers/license.txt for the
 * full text of the license. */

/**
 * @requires OpenLayers/Lang/en.js
 */

/**
 * Namespace: OpenLayers.Lang["ja"]
 * Dictionary for Japanese.  Keys for entries are used in calls to
 *     <OpenLayers.Lang.translate>.  Entry bodies are normal strings or
 *     strings formatted for use with <OpenLayers.String.format> calls.
 */
OpenLayers.Lang.ja = OpenLayers.Util.applyDefaults({

    'unhandledRequest': "未処理の要求が ${statusText} を返しました",

    'permalink': "Permalink",

    'overlays': "オーバーレイ",

    'baseLayer': "基準画層",

    'sameProjection':
        "オーバービュー マップは、メイン マップと同じ投影内にある場合のみ機能します",

    'readNotImplemented': "読み込みは実行されませんでした。",

    'writeNotImplemented': "書き込みは実行されませんでした。",

    'noFID': "FID が指定されていないフィーチャは更新できません。",

    'errorLoadingGML': "GML ファイル ${url} のロードでエラーが発生しました",

    'browserNotSupported':
        "ご使用のブラウザではベクトルのレンダリングがサポートされていません。現在サポートされているレンダラは次のとおりです:\n${renderers}",

    'componentShouldBe': "addFeatures : コンポーネントは ${geomType} である必要があります",

    // console message
    'getFeatureError':
        "レンダラがない画層で getFeatureFromEvent が呼び出されました。通常これは、画層が" +
        "壊れていて、それに関連付けられている一部のハンドラが壊れていないことを意味します。",

    // console message
    'minZoomLevelError':
        "minZoomLevel プロパティは、FixedZoomLevels の下位" +
        "画層で使用することを意図したものです。wfs 画層が " +
        "minZoomLevel をチェックすることはなくなりました。" +
        "ただしこのプロパティを削除するには、これに依存する" +
        " OL ベースのアプリケーションを削除する必要があります。" +
        "このため、minZoomLevel 以下のチェックは、3.0 で" +
        "廃止されます。代わりに最小/最大解像度設定を使用" +
        "してください。次のサイトを参照してください:" +
        "http://trac.openlayers.org/wiki/SettingZoomLevels",

    'commitSuccess': "WFS トランザクション: 成功 ${response}",

    'commitFailed': "WFS トランザクション: 失敗 ${response}",

    'googleWarning':
        "Google 画層を正しくロードできませんでした。<br><br>" +
        "このメッセージを非表示にするには、右上にある画層" +
        "スイッチ内の新しい BaseLayer を選択します。<br><br>" +
        "これは、Google Maps ライブラリ スクリプトが含まれて" +
        "いないか、サイトの正しい API キーが含まれていない" +
        "ことが原因であると考えられます。<br><br>" +
        "開発者の方へ: この作業を正しく実行するための情報は、" +
        "<a href='http://trac.openlayers.org/wiki/Google' " +
        "target='_blank'>ここ</a>をクリックしてください",

    'getLayerWarning':
        "${layerType} 画層を正しくロードできませんでした。<br><br>" +
        "このメッセージを非表示にするには、右上にある画層" +
        "スイッチ内の新しい BaseLayer を選択します。<br><br>" +
        "これは、${layerLib} ライブラリ スクリプトが正しく含まれ" +
        "ていないことが原因であると考えられます。<br><br>" +
        "開発者の方へ: この作業を正しく実行するための情報は、" +
        "<a href='http://trac.openlayers.org/wiki/${layerLib}' " +
        "target='_blank'>ここ</a>をクリックしてください",

    'scale': "尺度 = 1 : ${scaleDenom}",

    // console message
    'layerAlreadyAdded':
        "画層 ${layerName} をマップに追加しようとしましたが、この画層は既に追加されています",

    // console message
    'reprojectDeprecated':
        "'reproject' オプションを同じ ${layerName} 画層で使用して" +
        "います。このオプションは推奨しません。このオプションは商用の" +
        "ベースマップにデータを表示することを意図したものですが、" + 
        "その場合は Spherical Mercator サポートを使用してください。" +
        "詳細については、http://trac.openlayers.org/wiki/SphericalMercator" +
        "を参照してください。",

    // console message
    'methodDeprecated':
        "このメソッドは非推奨で、3.0 で廃止されます。 " +
        "代わりに ${newMethod} を使用してください。",

    // console message
    'boundsAddError': "関数を追加するには、x と y の両方の値を渡す必要があります。",

    // console message
    'lonlatAddError': "関数を追加するには、lon と lat の両方の値を渡す必要があります。",

    // console message
    'pixelAddError': "関数を追加するには、x と y の両方の値を渡す必要があります。",

    // console message
    'unsupportedGeometryType': "サポートされていないジオメトリ タイプ: ${geomType}",

    // console message
    'pagePositionFailed':
        "OpenLayers.Util.pagePosition が失敗しました: ID ${elemId} を持つエレメントは誤った場所に配置されている可能性があります。",
                    
    // console message
    'filterEvaluateNotImplemented': "このフィルタの種類では評価が実装されていません。",
	
	'end': ''
}, OpenLayers.Lang["en"]);
