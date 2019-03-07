$(function () {
    var $body = $('body');
    var $frames = $('#frames');
    var $hexInput = $('#hex-input');
    var $insertButton = $('#insert-button');
    var $deleteButton = $('#delete-button');
    var $updateButton = $('#update-button');
    var $runAnimationButton = $('#runAnimation-button');
    var $SaveFHatBadge = $('#SaveFHatbadge-menuItem');
    var $SavePC = $('#SavePC-menuItem');
    var $LoadFHaTbadge = $('#LoadFHatbadge-menuItem');
    var $LoadPC = $('#LoadPC-menuItem');


    var $leds, $cols, $rows;

    var generator = {
        tableCols: function () {
            var out = ['<table id="cols-list"><tr>'];
            for (var i = 1; i < 9; i++) {
                out.push('<td class="item" data-col="' + i + '">' + i + '</td>');
            }
            out.push('</tr></table>');
            return out.join('');
        },
        tableRows: function () {
            var out = ['<table id="rows-list">'];
            for (var i = 1; i < 9; i++) {
                out.push('<tr><td class="item" data-row="' + i + '">' + i + '</td></tr>');
            }
            out.push('</table>');
            return out.join('');
        },
        tableLeds: function () {
            var out = ['<table id="leds-matrix" tabindex="1">'];
            for (var i = 1; i < 9; i++) {
                out.push('<tr>');
                for (var j = 1; j < 9; j++) {
                    out.push('<td class="item" data-row="' + i + '" data-col="' + j + '"></td>');
                }
                out.push('</tr>');
            }
            out.push('</table>');
            return out.join('');
        }
    };

    var converter = {
        patternToFrame: function (pattern) {
            var out = ['<table class="frame" data-hex="' + pattern + '">'];
            for (var i = 1; i < 9; i++) {
                var byte = pattern.substr(-2 * i, 2);
                byte = parseInt(byte, 16);

                out.push('<tr>');
                for (var j = 0; j < 8; j++) {
                    if ((byte & 1 << j)) {
                        out.push('<td class="item active"></td>');
                    } else {
                        out.push('<td class="item"></td>');
                    }
                }
                out.push('</tr>');
            }
            out.push('</table>');
            return out.join('');
        },
        patternsToCodeUint64Array: function (patterns) {
            var out = ['const uint64_t IMAGES[] = {\n'];

            for (var i = 0; i < patterns.length; i++) {
                out.push('  0x');
                out.push(patterns[i]);
                out.push(',\n');
            }
            out.pop();
            out.push('\n};\n');
            out.push('const int IMAGES_LEN = sizeof(IMAGES)/8;\n');

            return out.join('');
        },
        patternsToCodeBytesArray: function (patterns) {
            var out = ['const byte IMAGES[][8] = {\n'];

            for (var i = 0; i < patterns.length; i++) {
                out.push('{\n');
                for (var j = 7; j >= 0; j--) {
                    var byte = patterns[i].substr(2 * j, 2);
                    byte = parseInt(byte, 16).toString(2);
                    byte = ('00000000' + byte).substr(-8);
                    byte = byte.split('').reverse().join('');
                    out.push('  B');
                    out.push(byte);
                    out.push(',\n');
                }
                out.pop();
                out.push('\n}');
                out.push(',');
            }
            out.pop();
            out.push('};\n');
            out.push('const int IMAGES_LEN = sizeof(IMAGES)/8;\n');
            return out.join('');
        },
        fixPattern: function (pattern) {
            pattern = pattern.replace(/[^0-9a-fA-F]/g, '0');
            return ('0000000000000000' + pattern).substr(-16);
        },
        fixPatterns: function (patterns) {
            for (var i = 0; i < patterns.length; i++) {
                patterns[i] = converter.fixPattern(patterns[i]);
            }
            return patterns;
        }
    };

    function makeFrameElement(pattern) {
        pattern = converter.fixPattern(pattern);
        return $(converter.patternToFrame(pattern)).click(onFrameClick);
    }

    function ledsToHex() {
        var out = [];
        for (var i = 1; i < 9; i++) {
            var byte = [];
            for (var j = 1; j < 9; j++) {
                var active = $leds.find('.item[data-row=' + i + '][data-col=' + j + '] ').hasClass('active');
                byte.push(active ? '1' : '0');
            }
            byte.reverse();
            byte = parseInt(byte.join(''), 2).toString(16);
            byte = ('0' + byte).substr(-2);
            out.push(byte);
        }
        out.reverse();
        $hexInput.val(out.join(''));
    }

    function hexInputToLeds() {
        var val = getInputHexValue();
        for (var i = 1; i < 9; i++) {
            var byte = val.substr(-2 * i, 2);

            byte = parseInt(byte, 16);
            for (var j = 1; j < 9; j++) {
                var active = !!(byte & 1 << (j - 1));
                $leds.find('.item[data-row=' + i + '][data-col=' + j + '] ').toggleClass('active', active);
            }
        }
    }

    var savedHashState;

    function printArduinoCode(patterns) {
        if (patterns.length) {
            var code;
            if ($('#images-as-byte-arrays').prop("checked")) {
                code = converter.patternsToCodeBytesArray(patterns);
            } else {
                code = converter.patternsToCodeUint64Array(patterns);
            }
            $('#output').val(code);
        }
    }

    function framesToPatterns() {
        var out = [];
        $frames.find('.frame').each(function () {
            out.push($(this).attr('data-hex'));
        });
        return out;
    }

    function saveState() {
        var patterns = framesToPatterns();
        printArduinoCode(patterns);
        window.location.hash = savedHashState = patterns.join('|');
    }

    function loadState() {
        savedHashState = window.location.hash.slice(1);
        $frames.empty();
        var frame;
        var patterns = savedHashState.split('|');
        patterns = converter.fixPatterns(patterns);

        for (var i = 0; i < patterns.length; i++) {
            frame = makeFrameElement(patterns[i]);
            $frames.append(frame);
        }
        frame.addClass('selected');
        $hexInput.val(frame.attr('data-hex'));
        printArduinoCode(patterns);
        hexInputToLeds();
    }

    function getInputHexValue() {
        return converter.fixPattern($hexInput.val());
    }

    function onFrameClick() {
        $hexInput.val($(this).attr('data-hex'));
        processToSave($(this));
        hexInputToLeds();
    }

    function processToSave($focusToFrame) {
        $frames.find('.frame.selected').removeClass('selected');

        if ($focusToFrame.length) {
            $focusToFrame.addClass('selected');
            $deleteButton.removeAttr('disabled');
            $updateButton.removeAttr('disabled');
        } else {
            $deleteButton.attr('disabled', 'disabled');
            $updateButton.attr('disabled', 'disabled');
        }
        $leds.focus();
        saveState();
    }

    function focusToFrame($focusToFrame) {
        $frames.find('.frame.selected').removeClass('selected');

        if ($focusToFrame.length) {
            $focusToFrame.addClass('selected');
            $deleteButton.removeAttr('disabled');
            $updateButton.removeAttr('disabled');
        } else {
            $deleteButton.attr('disabled', 'disabled');
            $updateButton.attr('disabled', 'disabled');
        }
        $leds.focus();
    }

    $('#cols-container').append($(generator.tableCols()));
    $('#rows-container').append($(generator.tableRows()));
    $('#leds-container').append($(generator.tableLeds()));

    $cols = $('#cols-list');
    $rows = $('#rows-list');
    $leds = $('#leds-matrix');

    var edit_mode = 0; // 0: none, 1: activate, 2: deactivate, 3: toggle

    $leds.find('.item').mousedown(function (e) {
        cachePattern();
        if (e.shiftKey) {
            edit_mode = 3;
        } else if ($(this).is(".active")) {
            edit_mode = 2;
        } else {
            edit_mode = 1;
        }
        $(this).toggleClass('active');
        ledsToHex();
    });

    $("#leds-container").mouseleave(function () {
        edit_mode = 0;
    }).mouseup(function () {
        edit_mode = 0;
    });

    $leds.find('.item').mouseenter(function () {
        if (edit_mode == 1) {
            $(this).toggleClass('active', true);
        } else if (edit_mode == 2) {
            $(this).toggleClass('active', false);
        } else if (edit_mode == 3) {
            $(this).toggleClass('active');
        } else {
            return;
        }
        ledsToHex();
    });

    var patternTool = {
        rotate: function (pattern, direction) {
            var byte_table = [];
            for (var i = 7; i >= 0; i--) {
                byte_table.push(parseInt(pattern.substr(2 * i, 2), 16));
            }
            var rot = [];
            for (var i = 0; i < 8; i++) {
                var byte = 0;
                for (var j = 0; j < 8; j++) {
                    if (direction) {
                        if (byte_table[7-j] >> i & 1) {
                            byte |= 1 << j;
                        }
                    } else {
                        if (byte_table[j] >> (7-i) & 1) {
                            byte |= 1 << j;
                        }
                    }
                }
                rot.push(('0' + byte.toString(16)).substr(-2));
            }
            return rot.reverse().join('');
        },
        flipH: function (pattern) {
            var flip = [];
            for (var i = 0; i < 8; i++) {
                var byte = pattern.substr(2 * i, 2);
                byte = parseInt(byte, 16).toString(2);
                byte = ('00000000' + byte).substr(-8);
                byte = byte.split('').reverse().join('');
                byte = parseInt(byte, 2).toString(16);
                byte = ('0' + byte).substr(-2);
                flip.push(byte);
            }
            return flip.join('');
        },
        flipV: function (pattern) {
            return pattern.match(/.{2}/g).reverse().join('');
        },
        up: function (pattern, cyclic) {
            return (cyclic ? pattern.substr(14, 2) : '00') + pattern.substr(0, 14);
        },
        down: function (pattern, cyclic) {
            return pattern.substr(2, 14) + (cyclic ? pattern.substr(0, 2) : '00');
        },
        right: function (pattern, cyclic) {
            var out = [];
            for (var i = 0; i < 8; i++) {
                var byte = pattern.substr(i * 2, 2);
                byte = parseInt(byte, 16);
                if (cyclic) {
                    byte = (byte << 1) | (byte >> 7);
                } else {
                    byte <<= 1;
                }
                byte = byte.toString(16);
                byte = ('0' + byte).substr(-2);
                out.push(byte);
            }
            return out.join('');
        },
        left: function (pattern, cyclic) {
            var out = [];
            for (var i = 0; i < 8; i++) {
                var byte = pattern.substr(i * 2, 2);
                byte = parseInt(byte, 16);
                if (cyclic) {
                    byte = (byte >> 1) | (byte << 7);
                } else {
                    byte >>= 1;
                }
                byte = byte.toString(16);
                byte = ('0' + byte).substr(-2);
                out.push(byte);
            }
            return out.join('');
        },
        not: function (pattern) {
            var out = [];
            for (var i = 0; i < 8; i++) {
                var byte = pattern.substr(i * 2, 2);
                byte = ~parseInt(byte, 16);
                byte = byte.toString(16);
                byte = ('0' + byte).substr(-2);
                out.push(byte);
            }
            return out.join('');
        },
        or: function (pattern1, pattern2) {
            var out = [];
            for (var i = 0; i < 8; i++) {
                var byte1 = pattern1.substr(i * 2, 2);
                var byte2 = pattern2.substr(i * 2, 2);
                var byte = parseInt(byte1, 16) | parseInt(byte2, 16);
                byte = byte.toString(16);
                byte = ('0' + byte).substr(-2);
                out.push(byte);
            }
            return out.join('');
        },
        xor: function (pattern1, pattern2) {
            var out = [];
            for (var i = 0; i < 8; i++) {
                var byte1 = pattern1.substr(i * 2, 2);
                var byte2 = pattern2.substr(i * 2, 2);
                var byte = parseInt(byte1, 16) ^ parseInt(byte2, 16);
                byte = byte.toString(16);
                byte = ('0' + byte).substr(-2);
                out.push(byte);
            }
            return out.join('');
        },
        and: function (pattern1, pattern2) {
            var out = [];
            for (var i = 0; i < 8; i++) {
                var byte1 = pattern1.substr(i * 2, 2);
                var byte2 = pattern2.substr(i * 2, 2);
                var byte = parseInt(byte1, 16) & parseInt(byte2, 16);
                byte = byte.toString(16);
                byte = ('0' + byte).substr(-2);
                out.push(byte);
            }
            return out.join('');
        }
    }

    $('#rotate-button').click(function () {
        cachePattern();
        $hexInput.val(patternTool.rotate(getInputHexValue(), true));
        hexInputToLeds();
    });

    $('#invert-button').click(function () {
        $leds.find('.item').toggleClass('active');
        ledsToHex();
    });

    $('#shift-up-button').click(function () {
        cachePattern();
        $hexInput.val(patternTool.up(getInputHexValue()));
        hexInputToLeds();
    });

    $('#shift-down-button').click(function () {
        cachePattern();
        $hexInput.val(patternTool.down(getInputHexValue()));
        hexInputToLeds();
    });

    $('#shift-right-button').click(function () {
        cachePattern();
        $hexInput.val(patternTool.right(getInputHexValue()));
        hexInputToLeds();
    });

    $('#shift-left-button').click(function () {
        cachePattern();
        $hexInput.val(patternTool.left(getInputHexValue()));
        hexInputToLeds();
    });

    var patternClipboard = 0;
    var patternHistory = [];
    function cachePattern() {
        patternHistory.push($hexInput.val());
        while (patternHistory.length > 50) {
            patternHistory.shift();
        }
    }

    $leds.keydown(function (e) {
        if (e.keyCode == 39) {  // arrow right
            cachePattern();
            if (e.ctrlKey) {
                $hexInput.val(patternTool.rotate(getInputHexValue(), true));
                hexInputToLeds();
            } else {
                $hexInput.val(patternTool.right(getInputHexValue(), e.shiftKey));
                hexInputToLeds();
            }
        } else if (e.keyCode == 37) {  // arrow left
            cachePattern();
            if (e.ctrlKey) {
                $hexInput.val(patternTool.rotate(getInputHexValue(), false));
                hexInputToLeds();
            } else {
                $hexInput.val(patternTool.left(getInputHexValue(), e.shiftKey));
                hexInputToLeds();
            }
        } else if (e.keyCode == 38) {  // arrow up
            cachePattern();
            if (e.ctrlKey) {
                $hexInput.val(patternTool.flipV(getInputHexValue()));
                hexInputToLeds();
            } else {
                $hexInput.val(patternTool.up(getInputHexValue(), e.shiftKey));
                hexInputToLeds();
            }
        } else if (e.keyCode == 40) {  // arrow down
            cachePattern();
            if (e.ctrlKey) {
                $hexInput.val(patternTool.flipV(getInputHexValue()));
                hexInputToLeds();
            } else {
                $hexInput.val(patternTool.down(getInputHexValue(), e.shiftKey));
                hexInputToLeds();
            }
        } else if (e.keyCode == 67 && e.ctrlKey) {  // Ctrl-C
            patternClipboard = getInputHexValue();
        } else if (e.keyCode == 86 && e.ctrlKey) {  // Ctrl-V
            cachePattern();
            var val;
            if (e.shiftKey) {
                val = patternTool.xor(getInputHexValue(), patternClipboard);
            } else {
                val = patternTool.or(getInputHexValue(), patternClipboard);
            }
            $hexInput.val(val);
            hexInputToLeds();
        } else if (e.keyCode == 90 && e.ctrlKey) {  // Ctrl-Z
            $hexInput.val(patternHistory.pop());
            hexInputToLeds();
        } else if (e.keyCode == 33) {  // page up
            if (e.altKey) {
                var $thisFrame = $frames.find('.frame.selected').first();
                var $prevFrame = $thisFrame.prev('.frame');
                if ($prevFrame.length) {
                    $prevFrame.before($thisFrame);
                    saveState();
                }
            } else {
                var $prevFrame = $frames.find('.frame.selected').first().prev('.frame');
                if ($prevFrame.length) {
                    $hexInput.val($prevFrame.attr('data-hex'));
                    focusToFrame($prevFrame);
                    hexInputToLeds();
                }
            }
        } else if (e.keyCode == 34) {  // page down
            if (e.altKey) {
                var $thisFrame = $frames.find('.frame.selected').first();
                var $nextFrame = $thisFrame.next('.frame');
                if ($nextFrame.length) {
                    $nextFrame.after($thisFrame);
                    saveState();
                }
            } else {
                var $nextFrame = $frames.find('.frame.selected').first().next('.frame');
                if ($nextFrame.length) {
                    $hexInput.val($nextFrame.attr('data-hex'));
                    focusToFrame($nextFrame);
                    hexInputToLeds();
                }
            }
        } else if (e.keyCode == 32) {  // space
            $("#play-button").click();
        } else if (e.keyCode == 13) {  // enter
            if (e.ctrlKey) {
                $updateButton.click();
            } else {
                $insertButton.click();
            }
        } else if (e.keyCode == 46) {  // delete
            $deleteButton.click();
        } else {
            return;
        }
        e.preventDefault();
    });

    $runAnimationButton.click(function (){
        var params = 'pattern=' + savedHashState+ '&delay=' + $('#play-delay-input').val();
        var xhttp = new XMLHttpRequest();
        	xhttp.open("POST", "/pattern" , true);
            xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
        	xhttp.send(params);
    });
    $SaveFHatBadge.click(function (){
        var fileList;
        var xhtp = new XMLHttpRequest();
        var params = '/directory=' + "DummyData";
        xhtp.open("Get", "/directory" , true);
        xhtp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
        xhtp.send(params);
        xhtp.onload = function(e){
            fileList = xhtp.responseText;
            var fileName = prompt(fileList+"\r\n\r\nPlease enter the file name:", "matrix.FHaT");
            if (fileName !== null){
                var params = 'saveData=' + savedHashState+ '&delay=' + $('#play-delay-input').val() + '&fileName=' + fileName;
                var xhttp = new XMLHttpRequest();
        	    xhttp.open("POST", "/save" , true);
                xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
                xhttp.send(params);
            }
        };
    });
    $SavePC.click(function(){
        var filename ="matrix.FHaT";
        var data = savedHashState + '|' + $('#play-delay-input').val();
        var file = new Blob([data], {type: "text/plain"});
    if (window.navigator.msSaveOrOpenBlob) // IE10+
        window.navigator.msSaveOrOpenBlob(file, filename);
    else { // Others
        var a = document.createElement("a"),
                url = URL.createObjectURL(file);
        a.href = url;
        a.download = filename;
        document.body.appendChild(a);
        a.click();
        setTimeout(function() {
            document.body.removeChild(a);
            window.URL.revokeObjectURL(url);  
        }, 0); 
    }
    });
    $LoadFHaTbadge.click(function(){
        var fileList;
        var xhtp = new XMLHttpRequest();
        var params = '/directory=' + "DummyData";
        xhtp.open("Get", "/directory" , true);
        xhtp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
        xhtp.send(params);
        xhtp.onload = function(e){
            fileList = xhtp.responseText;
            var fileName = prompt(fileList+"\r\n\r\nPlease enter the file name:", "matrix.FHaT");
            if (fileName !== null){
                if (!fileName.endsWith(".FHaT")){ 
                    fileName += ".FHaT";
                };
                var params = 'filename=' + fileName;
                var xhttp = new XMLHttpRequest();
        	    xhttp.open("Get", fileName , true);
                xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
                xhttp.send(params);
                xhttp.onload = function(e){
                    var theData = xhttp.responseText;
                    savedHashState = "#"+theData;
                    $('#play-delay-input').val(savedHashState.substring(savedHashState.lastIndexOf("|")+1,savedHashState.length));
                    window.location.hash = savedHashState.substring(0,savedHashState.lastIndexOf("|"));
                };
            };
        };
    });
    $LoadPC.click(function(){
        // Create an input element
    var inputElement = document.createElement("input");
    inputElement.type = "file";
    inputElement.accept = "*.FHaT";
    // set onchange event to call callback when user has selected file
    inputElement.addEventListener("change", function(event){
            var reader = new FileReader();
            reader.onload = (function(fileLoadedEvent){
                savedHashState = "#"+fileLoadedEvent.target.result;
                $('#play-delay-input').val(savedHashState.substring(savedHashState.lastIndexOf("|")+1,savedHashState.length));
                window.location.hash = savedHashState.substring(0,savedHashState.lastIndexOf("|"));
            });
            reader.readAsText(this.files[0], "UTF-8");
    });
    // dispatch a click event to open the file dialog
    inputElement.dispatchEvent(new MouseEvent("click")); 
    });

    $cols.find('.item').mousedown(function (e) {
        cachePattern();
        var col = $(this).attr('data-col');
        if (e.shiftKey) {
            $leds.find('.item[data-col=' + col + ']').toggleClass('active');
        } else {
            $leds.find('.item[data-col=' + col + ']').toggleClass('active',
                $leds.find('.item[data-col=' + col + '].active').length != 8);
        }
        ledsToHex();
    });

    $rows.find('.item').mousedown(function (e) {

        cachePattern();
        var row = $(this).attr('data-row');
        if (e.shiftKey) {
            $leds.find('.item[data-row=' + row + ']').toggleClass('active');
        } else {
            $leds.find('.item[data-row=' + row + ']').toggleClass('active',
                $leds.find('.item[data-row=' + row + '].active').length != 8);
        }
        ledsToHex();
    });

    $hexInput.keyup(function () {
        cachePattern();
        hexInputToLeds();
    });

    $deleteButton.click(function () {
        var $selectedFrame = $frames.find('.frame.selected').first();
        var $nextFrame = $selectedFrame.next('.frame').first();

        if (!$nextFrame.length) {
            $nextFrame = $selectedFrame.prev('.frame').first();
        }

        $selectedFrame.remove();

        if ($nextFrame.length) {
            $hexInput.val($nextFrame.attr('data-hex'));
        }

        processToSave($nextFrame);

        hexInputToLeds();
    });

    $insertButton.click(function () {
        var $newFrame = makeFrameElement(getInputHexValue());
        var $selectedFrame = $frames.find('.frame.selected').first();

        if ($selectedFrame.length) {
            $selectedFrame.after($newFrame);
        } else {
            $frames.append($newFrame);
        }

        processToSave($newFrame);
    });

    $updateButton.click(function () {
        var $newFrame = makeFrameElement(getInputHexValue());
        var $selectedFrame = $frames.find('.frame.selected').first();

        if ($selectedFrame.length) {
            $selectedFrame.replaceWith($newFrame);
        } else {
            $frames.append($newFrame);
        }

        processToSave($newFrame);
    });

    $('#images-as-byte-arrays').change(function () {
        var patterns = framesToPatterns();
        printArduinoCode(patterns);
    });


    $('#matrix-toggle').hover(function () {
        $cols.find('.item').addClass('hover');
        $rows.find('.item').addClass('hover');
    }, function () {
        $cols.find('.item').removeClass('hover');
        $rows.find('.item').removeClass('hover');
    });

    $('#matrix-toggle').mousedown(function (e) {
        cachePattern();
        var col = $(this).attr('data-col');
        if (e.shiftKey) {
            $leds.find('.item').toggleClass('active');
        } else {
            $leds.find('.item').toggleClass('active', $leds.find('.item.active').length != 64);
        }
        ledsToHex();
    });

    $('#circuit-theme').click(function () {
        if ($body.hasClass('circuit-theme')) {
            $body.removeClass('circuit-theme');
            Cookies.set('page-theme', 'plain-theme', {path: ''});
        } else {
            $body.addClass('circuit-theme');
            Cookies.set('page-theme', 'circuit-theme', {path: ''});
        }
    });

    $('.leds-case').click(function () {
        var themeName = $(this).attr('data-leds-theme');
        setLedsTheme(themeName);
        Cookies.set('leds-theme', themeName, {path: ''});
    });

    function setLedsTheme(themeName) {
        $body.removeClass('red-leds yellow-leds green-leds blue-leds white-leds').addClass(themeName);
    }

    function setPageTheme(themeName) {
        $body.removeClass('plain-theme circuit-theme').addClass(themeName);
    }

    var playInterval;

    $('#play-button').click(function () {
        if (playInterval) {
            $('#play-button-stop').hide();
            $('#play-button-play').show();
            clearInterval(playInterval);
            playInterval = null;
        } else {
            $('#play-button-stop').show();
            $('#play-button-play').hide();

            playInterval = setInterval(function () {
                var $selectedFrame = $frames.find('.frame.selected').first();
                var $nextFrame = $selectedFrame.next('.frame').first();

                if (!$nextFrame.length) {
                    $nextFrame = $frames.find('.frame').first();
                }

                if ($nextFrame.length) {
                    $hexInput.val($nextFrame.attr('data-hex'));
                }

                processToSave($nextFrame);

                hexInputToLeds();
            }, $('#play-delay-input').val());
        }
    });


    $(window).on('hashchange', function () {
        if (window.location.hash.slice(1) != savedHashState) {
            loadState();
        }
    });

    $frames.sortable({
        stop: function (event, ui) {
            saveState();
        }
    });

    loadState();

    var ledsTheme = Cookies.get('leds-theme');

    if (ledsTheme) {
        setLedsTheme(ledsTheme);
    }

    var pageTheme = Cookies.get('page-theme') || 'circuit-theme';

    setPageTheme(pageTheme);

    $leds.focus();
});
