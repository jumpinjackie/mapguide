/**
 * @project         Jx
 * @revision        $Id: jxgrid.js 342 2007-06-29 20:01:46Z pspencer $
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 */

/* our default css styles */
Jx.addStyleSheet('grid/grid.css');

/**
 * Jx.Grid is a tabular control with convenient controls for resizing columns,
 * sorting, and inline editing.  It is created inside another element, typically a
 * div.  If the div is resizable (for instance it fills the page or there is a
 * user control allowing it to be resized), you must call the resize() method
 * of the grid to let it know that its container has been resized.
 *
 * When creating a new Jx.Grid, you can specify a number of options for the grid
 * that control its appearance and functionality.
 *
 * Jx.Grid renders data that comes from an external source.  This external 
 * source, called the model, must implement the following interface:
 *
 * addGridListener(l)
 * mandatory.  This function accepts one argument, l, which is the listener
 * to add.  The model can then call the gridChanged() method on the grid
 * listener object when something in the model changes.
 * 
 * removeGridListener(l)
 * mandatory.  This function accepts one argument, l, which is the listener
 * to remove.  The listener should have been previously added using
 * addGridListener.
 * 
 * getColumnCount()
 * mandatory.  This function returns the number of columns of data in the 
 * model as an integer value.
 * 
 * getColumnHeaderHTML(column)
 * mandatory. This function returns an HTML string to be placed in the
 * column header for the given column index.
 * 
 * getColumnHeaderHeight();
 * mandatory.  This function returns an integer which is the height of the
 * column header row in pixels.
 * 
 * getColumnWidth(column)
 * mandatory.  This function returns an integer which is the width of the
 * given column in pixels.
 * 
 * setColumnWidth(column, width)
 * optional.  This function is called with a column index and width in pixels
 * when a column is resized.  This function is only required if the grid
 * allows resizeable columns.
 * 
 * getRowHeaderHTML(row)
 * mandatory.  This function returns an HTML string to be placed in the row
 * header for the given row index
 * 
 * getRowHeaderWidth()
 * mandatory.  This function returns an integer which is the width of the row
 * header column in pixels.
 * 
 * getRowHeight(row)
 * mandatory.  This function returns an integer which is the height of the
 * given row in pixels.
 * 
 * getRowCount()
 * mandatory.  This function returns the number of rows of data in the model
 * as an integer value.
 * 
 * getValueAt(row, column)
 * mandatory.  This function returns an HTML string which is the text to place
 * in the cell at the given row and column.
 * 
 * isCellEditable(row, column) 
 * mandatory.  This function returns a boolean value to indicate if a given
 * cell is editable by the user.
 * 
 * setValueAt(row, column, value) 
 * optional.  This function is called with the row and column of a cell and a
 * new value for the cell.  It is mandatory to provide this function if any of
 * the cells in the model are editable.
 * 
 * rowSelected(row)
 * optional.  This function is called by the grid to indicate that the user
 * has selected a row by clicking on the row header.
 * 
 * columnSelected(column)
 * optional.  This function is called by the grid to indicate that the user
 * has selected a column by clicking on the column header.
 * 
 * cellSelected(row, column)
 * optional.  This function is called by the grid to indicate that the user
 * has selected a cell by clicking on the cell in the grid.
 */
Jx.Grid = Class.create();
Jx.Grid.prototype = {
    domObj : null,
    model : null,
    /**
     * construct a new instance of Jx.Grid within the domObj
     */
    initialize : function( domObj, options ) {
        this.domObj = $(domObj);
        
        /* if this grid is the content of a Jx Panel, this will
           allow the grid to resize when the panel resizes
         */
        if (!this.domObj.jxLayout) {
            new Jx.Layout(this.domObj);
        }
        this.domObj.jxLayout.addSizeChangeListener(this);
        
        options = options || {};
        
        this.rowColObj = document.createElement('div');
        this.rowColObj.className = 'jxGridContainer';
        
        this.colObj = document.createElement('div');
        this.colObj.className = 'jxGridContainer';
        this.colTable = document.createElement('table');
        this.colTable.className = 'jxGridTable';
        this.colTableHead = document.createElement('thead');
        this.colTable.appendChild(this.colTableHead);
        this.colTableBody = document.createElement('tbody');
        this.colTable.appendChild(this.colTableBody);
        this.colObj.appendChild(this.colTable);
        
        this.rowObj = document.createElement('div');
        this.rowObj.className = 'jxGridContainer';
        this.rowTable = document.createElement('table');
        this.rowTable.className = 'jxGridTable';
        this.rowTableHead = document.createElement('thead');
        this.rowTable.appendChild(this.rowTableHead);
        this.rowObj.appendChild(this.rowTable);
        
        this.gridObj = document.createElement('div');
        this.gridObj.className = 'jxGridContainer';
        this.gridObj.style.overflow = 'scroll';
        this.gridTable = document.createElement('table');
        this.gridTable.className = 'jxGridTable';
        this.gridTableBody = document.createElement('tbody');
        this.gridTable.appendChild(this.gridTableBody);
        this.gridObj.appendChild(this.gridTable);
        
        this.domObj.appendChild(this.rowColObj);
        this.domObj.appendChild(this.rowObj);
        this.domObj.appendChild(this.colObj);
        this.domObj.appendChild(this.gridObj);
        
        this.bAlternateRowColors = options.alternateRowColors || false;
        this.showRowHeader = options.rowHeaders || false;
        this.showColumnHeader = options.columnHeaders || false;
        this.rowSelection = options.rowSelection || false;
        this.cellSelection = options.cellSelection || false;
                
        Event.observe(this.gridObj, 'scroll', this.onScroll.bind(this));
        Event.observe(this.gridObj, 'click', this.onClickGrid.bindAsEventListener(this));
        Event.observe(this.rowObj, 'click', this.onClickRowHeader.bindAsEventListener(this));
        Event.observe(this.colObj, 'click', this.onClickColumnHeader.bindAsEventListener(this));
        Event.observe(this.gridObj, 'mousemove', this.onMouseMoveGrid.bindAsEventListener(this));
        Event.observe(this.rowObj, 'mousemove', this.onMouseMoveRowHeader.bindAsEventListener(this));
        Event.observe(this.colObj, 'mousemove', this.onMouseMoveColumnHeader.bindAsEventListener(this));
    },
    
    /**
     * handle the grid scrolling by updating the position of the headers
     */
    onScroll: function() {
        this.colObj.scrollLeft = this.gridObj.scrollLeft;
        this.rowObj.scrollTop = this.gridObj.scrollTop;        
    },
    
    sizeChanged: function() {
        this.resize();
    },
    
    /**
     * resize the grid to fit inside its container.  This involves knowing something
     * about the model it is displaying (the height of the column header and the
     * width of the row header) so nothing happens if no model is set
     */
    resize: function() {
        if (!this.model) {
            return;
        }
        
        /* TODO: if not showing column or row, do this differently */
        var colHeight = this.showColumnHeader ? this.model.getColumnHeaderHeight() : 1;
        var rowWidth = this.showRowHeader ? this.model.getRowHeaderWidth() : 1;
        
        var size = Element.getContentBoxSize(this.domObj);
        
        /* -1 because of the right/bottom borders */
        this.rowColObj.style.width = (rowWidth - 1) + 'px';
        this.rowColObj.style.height = (colHeight - 1) + 'px';
        
        this.rowObj.style.top = (colHeight) + 'px';
        this.rowObj.style.left = '0px';
        this.rowObj.style.width = (rowWidth - 1) + 'px';
        this.rowObj.style.height = (size.height - colHeight - 1) + 'px';

        this.colObj.style.top = '0px';
        this.colObj.style.left = (rowWidth) + 'px';
        this.colObj.style.width = (size.width - rowWidth - 1) + 'px';
        this.colObj.style.height = (colHeight - 1) + 'px';

        this.gridObj.style.top = (colHeight) + 'px';
        this.gridObj.style.left = (rowWidth) + 'px';
        this.gridObj.style.width = (size.width - rowWidth - 1) + 'px';
        this.gridObj.style.height = (size.height - colHeight - 1) + 'px';
    },
    
    /**
     * set the model for the grid to display.  If a model is attached to the grid
     * it is removed and the new model is displayed.
     */
    setModel: function(model) {
        if (this.model) {
            this.model.removeGridListener(this);
        }
        this.model = model;
        if (this.model) {
            this.domObj.jxLayout.resize();
            this.model.addGridListener(this);
            this.createGrid();
            this.resize();
        } else {
            this.destroyGrid();
        }
    },
    
    /**
     * destroy the contents of the grid safely
     */
    destroyGrid: function() {
        var n = this.colTableHead.cloneNode(false);
        this.colTable.replaceChild(n, this.colTableHead);
        this.colTableHead = n;
        
        n = this.colTableBody.cloneNode(false);
        this.colTable.replaceChild(n, this.colTableBody);
        this.colTableBody = n;
        
        n = this.rowTableHead.cloneNode(false);
        this.rowTable.replaceChild(n, this.rowTableHead);
        this.rowTableHead = n;
        
        n = this.gridTableBody.cloneNode(false);
        this.gridTable.replaceChild(n, this.gridTableBody);
        this.gridTableBody = n;
        
    },
    
    /**
     * create the grid for the current model
     */
    createGrid: function() {
        this.destroyGrid();
        if (this.model) {
            var model = this.model;
            var nColumns = model.getColumnCount();
            var nRows = model.getRowCount();
            
            /* create header if necessary */
            if (this.showColumnHeader) {
                var colHeight = model.getColumnHeaderHeight();
                var trHead = document.createElement('tr');
                this.colTableHead.appendChild(trHead);
                var trBody = document.createElement('tr');
                this.colTableBody.appendChild(trBody);
                
                var th = document.createElement('th');
                th.style.width = '0px';
                th.style.height = '0px';
                trHead.appendChild(th);
                th = th.cloneNode(true);
                th.style.height = (colHeight) + 'px';
                trBody.appendChild(th);
                for (var i=0; i<nColumns; i++) {
                    var colWidth = model.getColumnWidth(i);
                    th = document.createElement('th');
                    th.className = 'jxGridColHeadHide';
                    th.style.width = (colWidth) + 'px';
                    var p = document.createElement('p');
                    p.style.height = '0px';
                    p.style.width = (colWidth) + 'px';
                    th.appendChild(p);
                    trHead.appendChild(th);
                    
                    th = document.createElement('th');
                    th.className = 'jxGridColHead';
                    th.innerHTML = model.getColumnHeaderHTML(i);
                    trBody.appendChild(th);
                }
                /* one extra column at the end for filler */
                var th = document.createElement('th');
                th.style.width = '1000px';
                th.style.height = '0px';
                trHead.appendChild(th);
                th = th.cloneNode(true);
                th.style.height = (colHeight - 1) + 'px';
                th.className = 'jxGridColHead';
                trBody.appendChild(th);
                
            }
            
            if (this.showRowHeader) {
                var rowWidth = model.getRowHeaderWidth();
                var tr = document.createElement('tr');
                var td = document.createElement('td');
                td.style.width = '0px';
                td.style.height = '0px';
                tr.appendChild(td);
                var th = document.createElement('th');
                th.style.width = (rowWidth) + 'px';
                th.style.height = '0px';
                tr.appendChild(th);
                this.rowTableHead.appendChild(tr);
                for (var i=0; i<nRows; i++) {
                    var rowHeight = model.getRowHeight(i);
                    var tr = document.createElement('tr');
                    var td = document.createElement('td');
                    td.className = 'jxGridRowHeadHide';
                    td.style.width = '0px';
                    td.style.height = (rowHeight)+'px';
                    var p = document.createElement('p');
                    p.style.width = '0px';
                    p.style.height = (rowHeight)+'px';
                    td.appendChild(p);
                    tr.appendChild(td);
                    var th = document.createElement('th');
                    th.className = 'jxGridRowHead';
                    th.innerHTML = model.getRowHeaderHTML(i);
                    tr.appendChild(th);
                    this.rowTableHead.appendChild(tr);
                }
                /* one extra row at the end for filler */
                var tr = document.createElement('tr');
                var td = document.createElement('td');
                td.style.width = '0px';
                td.style.height = '1000px';
                tr.appendChild(td);
                var th = document.createElement('th');
                th.style.width = (rowWidth) + 'px';
                th.style.height = '1000px';
                th.className = 'jxGridRowHead';
                tr.appendChild(th);
                this.rowTableHead.appendChild(tr);
            }
            
            var colHeight = model.getColumnHeaderHeight();
            var trBody = document.createElement('tr');
            this.gridTableBody.appendChild(trBody);
            
            var td = document.createElement('td');
            td.style.width = '0px';
            td.style.height = '0px';
            trBody.appendChild(td);
            for (var i=0; i<nColumns; i++) {
                var colWidth = model.getColumnWidth(i);
                td = document.createElement('td');
                td.className = 'jxGridColHeadHide';
                td.style.width = (colWidth) + 'px';
                var p = document.createElement('p');
                p.style.height = '0px';
                p.style.width = (colWidth) + 'px';
                td.appendChild(p);
                trBody.appendChild(td);
            }
            
            for (var j=0; j<nRows; j++) {
                var rowHeight = model.getRowHeight(j);
                var actualRowHeight = rowHeight;
                var tr = document.createElement('tr');
                this.gridTableBody.appendChild(tr);
                
                var td = document.createElement('td');
                td.className = 'jxGridRowHeadHide';
                td.style.width = '0px';
                td.style.height = (rowHeight) + 'px';
                var p = document.createElement('p');
                p.style.height = (rowHeight) + 'px';
                td.appendChild(p);
                tr.appendChild(td);
                for (var i=0; i<nColumns; i++) {
                    var colWidth = model.getColumnWidth(i);
                    td = document.createElement('td');
                    td.className = 'jxGridCell';
                    td.innerHTML = model.getValueAt(j,i);
                    tr.appendChild(td);
                    var tdSize = Element.getDimensions(td);
                    if (tdSize.height > actualRowHeight) {
                        actualRowHeight = tdSize.height;
                    }
                }
                /* some notes about row sizing
                 * In Safari, the height of a TR is always returned as 0
                 * In Safari, the height of any given TD is the height it would
                 * render at, not the actual height of the row
                 * In IE, the height is returned 1px bigger than any other browser
                 * Firefox just works
                 *
                 * So, for Safari, we have to measure every TD and take the highest one
                 * and if its IE, we subtract 1 from the overall height, making all
                 * browsers identical
                 *
                 * Using document.all is not a good hack for this
                 */
                if (document.all) {
                    actualRowHeight -= 1;
                }
                if (this.showRowHeader) {
                    this.setRowHeaderHeight(j, actualRowHeight);                    
                }
                /* if we apply the class before adding content, it
                 * causes a rendering error in IE (off by 1) that is 'fixed'
                 * when another class is applied to the row, causing dynamic
                 * shifting of the row heights
                 */
                if (this.bAlternateRowColors) {
                    tr.className = (j%2) ? 'jxGridRowOdd' : 'jxGridRowEven';
                } else {
                    tr.className = 'jxGridRowAll';
                }
            }
            
        }
    },
    
    /**
     * set the height of a row.  This is used internally to adjust the height of
     * the row header when cell contents wrap.  A limitation of the table structure
     * is that overflow: hidden on a td will work horizontally but not vertically
     */
    setRowHeaderHeight: function(row, height) {
        //this.rowTableHead.childNodes[row+1].childNodes[0].style.height = (height) + 'px';
        this.rowTableHead.childNodes[row+1].childNodes[0].childNodes[0].style.height = (height) + 'px';
    },
    
    /**
     * called through the grid listener interface when data has changed in the
     * underlying model
     */
    gridChanged: function(model, row, col, value) {
        if (this.model == model) {
            this.gridObj.childNodes[row].childNodes[col].innerHTML = value;
        }
    },
    
    /** 
     * apply the jxGridRowHeaderPrelight style to the header cell of a row.
     * This removes the style from the previously pre-lit row header.
     * 
     */
    prelightRowHeader: function(row) {
        var cell = (row >= 0 && row < this.rowTableHead.rows.length-1) ? this.rowTableHead.rows[row+1].cells[1] : null;
        if (this.prelitRowHeader != cell) {
            if (this.prelitRowHeader) {
                Element.removeClassName(this.prelitRowHeader, 'jxGridRowHeaderPrelight');
            }
            this.prelitRowHeader = cell;
            if (this.prelitRowHeader) {
                Element.addClassName(this.prelitRowHeader, 'jxGridRowHeaderPrelight');
            }
        }
    },
    
    /** 
     * apply the jxGridColumnHeaderPrelight style to the header cell of a column.
     * This removes the style from the previously pre-lit column header.
     * 
     */
    prelightColumnHeader: function(col) {
        if (this.colTableBody.rows.length == 0) {
            return;
        }
        var cell = (col >= 0 && col < this.colTableBody.rows[0].cells.length-1) ? this.colTableBody.rows[0].cells[col+1] : null;
        if (this.prelitColumnHeader != cell) {
            if (this.prelitColumnHeader) {
                Element.removeClassName(this.prelitColumnHeader, 'jxGridColumnHeaderPrelight');
            }
            this.prelitColumnHeader = cell;
            if (this.prelitColumnHeader) {
                Element.addClassName(this.prelitColumnHeader, 'jxGridColumnHeaderPrelight');
            }
        }
    },
    
    /** 
     * apply the jxGridRowPrelight style to row.
     * This removes the style from the previously pre-lit row.
     * 
     */
    prelightRow: function(row) {
        var tr = (row >= 0 && row < this.gridTableBody.rows.length-1) ? this.gridTableBody.rows[row+1] : null;
        
        if (this.prelitRow != row) {
            if (this.prelitRow) {
                Element.removeClassName(this.prelitRow, 'jxGridRowPrelight');
            }
            this.prelitRow = tr;
            if (this.prelitRow && !Element.hasClassName(this.prelitRow, 'jxGridRowSelected')) {
                this.prelightRowHeader(row);
                Element.addClassName(this.prelitRow, 'jxGridRowPrelight');
            }
        }
    },
    
    /** 
     * apply the jxGridColumnPrelight style to a column.
     * This removes the style from the previously pre-lit column.
     * 
     * Not Yet Implemented.
     */
    prelightColumn: function(col) {
        /* todo implement column prelighting (possibly) */
        this.prelightColumnHeader(col);
    },
    
    /** 
     * apply the jxGridCellPrelight style to a cell.
     * This removes the style from the previously pre-lit cell.
     */
    prelightCell: function(row, col) {
         var td = (row >=0 && col >=0 && row < this.gridTableBody.rows.length - 1 && col < this.gridTableBody.rows[row+1].cells.length - 1) ? this.gridTableBody.rows[row+1].cells[col+1] : null;
        if (this.prelitCell != td) {
            if (this.prelitCell) {
                Element.removeClassName(this.prelitCell, 'jxGridCellPrelight');
            }
            this.prelitCell = td;
            if (this.prelitCell) {
                Element.addClassName(this.prelitCell, 'jxGridCellPrelight');
                this.prelightRow(row);
                this.prelightColumn(col);
            }
        }    
    },
    
    /** 
     * Select a cell and apply the jxGridCellSelected style to it.
     * This deselects a previously selected cell.
     *
     * If the model supports cell selection, it should implement
     * a cellSelected function to receive notification of the selection.
     */
    selectCell: function(row, col, bSelected) {
         var td = (row >=0 && col >=0 && row < this.gridTableBody.rows.length - 1 && col < this.gridTableBody.rows[row+1].cells.length - 1) ? this.gridTableBody.rows[row+1].cells[col+1] : null;
         if (!td) {
             return;
         }
         
         if (this.selectedCell) {
             Element.addClassName(this.selectedCell, 'jxGridCellSelected');
         } else {
             Element.removeClassName(this.selectedCell, 'jxGridCellSelected');
         }
    },
    
    /** 
     * Apply the jxGridRowHeaderSelected style to the row header cell of a
     * selected row.
     */
    selectRowHeader: function(row, bSelected) {
        var cell = (row >= 0 && row < this.rowTableHead.rows.length-1) ? this.rowTableHead.rows[row+1].cells[1] : null;
        if (!cell) {
            return;
        }
        if (bSelected) {
            Element.addClassName(cell, 'jxGridRowHeaderSelected');
        } else {
            Element.removeClassName(cell, 'jxGridRowHeaderSelected');
        }
    },
    
    /** 
     * Select a row and apply the jxGridRowSelected style to it.
     *
     * If the model supports row selection, it should implement
     * a rowSelected function to receive notification of the selection.
     */
    selectRow: function(row, bSelected) {
        var tr = (row >= 0 && row < this.gridTableBody.rows.length - 1) ? this.gridTableBody.rows[row+1] : null;
        if (tr) {
            if (bSelected) {
                Element.addClassName(tr, 'jxGridRowSelected');
            } else {
                Element.removeClassName(tr, 'jxGridRowSelected');
            }
        }
        this.selectRowHeader(row, bSelected);
    },
    
    /** 
     * Apply the jxGridColumnHeaderSelected style to the column header cell of a
     * selected column.
     */
    selectColumnHeader: function(col, bSelected) {
        if (this.colTableBody.rows.length == 0) {
            return;
        }
        var cell = (col >= 0 && col < this.colTableBody.rows[0].cells.length-1) ? this.colTableBody.rows[0].cells[col+1] : null;
        if (cell == null) { 
            return; 
        }
        
        if (bSelected) {
            Element.addClassName(cell, 'jxGridColumnHeaderSelected');
        } else {
            Element.removeClassName(cell, 'jxGridColumnHeaderSelected');
        }
    },
    
    /** 
     * Select a column.
     * This deselects a previously selected column.
     */
    selectColumn: function(col, bSelected) {
        /* todo: implement column selection */
        if (col >= 0 && col < this.gridTable.rows[0].cells.length) {
            if (bSelected) {
                for (var i=0; i<this.gridTable.rows.length; i++) {
                    Element.removeClassName(this.gridTable.rows[i].cells[this.selectedColumn + 1], 'jxGridColumnSelected');
                }
            } else {
                for (var i=0; i<this.gridTable.rows.length; i++) {
                    Element.addClassName(this.gridTable.rows[i].cells[col+1], 'jxGridColumnSelected');
                }
                
            }
        }
        this.selectColumnHeader(col, bSelected);
    },
    
    /**
     * handle the mouse moving over the main grid.  This pre-lights the cell,
     * and subsquently the row and column (and headers).
     */
    onMouseMoveGrid: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        this.prelightCell(rc.row, rc.column);
    },
    
    onMouseMoveRowHeader: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        this.prelightRow(rc.row);
    },

    onMouseMoveColumnHeader: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        this.prelightColumn(rc.column);
    },
    
    /**
     * handle the user clicking on the grid.  This triggers cell selection
     * and ultimately row and column (and header) styling changes and an
     * event to the model (if a cellSelected function is provided)
     */
    onClickGrid: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        //this.selectCell(rc.row, rc.column);
        
        if (this.model.cellSelected) {
            this.model.cellSelected(this, rc.row, rc.column);
        }
    },
    
    /**
     * handle the user clicking on the row header.  This triggers row
     * selection and row (and header) styling changes and an
     * event to the model (if a rowSelected function is provided)
     */
    onClickRowHeader: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        //this.selectRow(rc.row);
        
        if (this.model.rowSelected) {
            this.model.rowSelected(this, rc.row);
        }
    },
    
    /**
     * handle the user clicking on the column header.  This triggers column
     * selection and column (and header) styling changes and an
     * event to the model (if a columnSelected function is provided)
     */
    onClickColumnHeader: function(e) {
        var rc = this.getRowColumnFromEvent(e);
        //this.selectColumn(rc.column);
        
        if (this.model.columnSelected) {
            this.model.columnSelected(this, rc.column);
        }
    },
    
    /**
     * retrieve the row and column indexes from an event click.
     * This function is used by the grid, row header and column
     * header to safely get these numbers.
     *
     * If the event isn't valid (i.e. it wasn't on a TD or TH) then
     * the returned values will be -1, -1
     *
     * @return Object an object with two properties, row and column,
     *         that contain the row and column that was clicked
     */
    getRowColumnFromEvent: function(e) {
        var td = Event.element(e);
        if (td.tagName != 'TD' && td.tagName != 'TH') {
            return {row:-1,column:-1};
        }
        var tr = td.parentNode;
        var col = td.cellIndex - 1; /* because of hidden spacer column */
        var row = tr.rowIndex - 1; /* because of hidden spacer row */
        
        if (col == -1) { 
            /* bug in safari returns 0 for cellIndex - only choice seems
             * to be to loop through the row
             */
            for (var i=0; i<tr.childNodes.length; i++) {
                if (tr.childNodes[i] == td) {
                    col = i - 1;
                    break;
                }
            }
        }
        return {row:row,column:col};
    }
};