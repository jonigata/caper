$(function(){ // on dom ready

    ////////////////////////////////////////////////////////////////
    // graph
    $('#cy').cytoscape({
        style: cytoscape.stylesheet()
            .selector('node')
            .css({
                'content': 'data(name)',
                'text-valign': 'center',
                'color': 'white',
                'text-outline-width': 2,
                'text-outline-color': 'data(faveColor)',
                'background-color': 'data(faveColor)'
            })
            .selector('edge')
            .css({
                'target-arrow-shape': 'triangle',
                'line-color': 'data(faveColor)',
                'target-arrow-color': 'data(faveColor)'
            })
            .selector(':selected')
            .css({
                'background-color': 'black',
                'line-color': 'black',
                'target-arrow-color': 'black',
                'source-arrow-color': 'black'
            })
            .selector('.faded')
            .css({
                'opacity': 0.25,
                'text-opacity': 0
            }),

        panningEnabled: true,
        userPanningenabled: true,
        boxSelectionEnabled: false,
        layout: {
            name: 'null'
        },

        elements: {
            nodes: [],
            edges: []
        },

        ready: function(){
            window.cy = this;
            // giddy up...
            
            cy.elements().unselectify();
            
            cy.on('tap', 'node', function(e){
                var node = e.cyTarget; 
                var neighborhood = node.neighborhood().add(node);
                
                cy.elements().addClass('faded');
                neighborhood.removeClass('faded');
            });
            
            cy.on('tap', function(e){
                if( e.cyTarget === cy ){
                    cy.elements().removeClass('faded');
                }
            });
        }
    });

    ////////////////////////////////////////////////////////////////
    // parser
    var data = [
        [caper_parser.Token.token_n, "I"],
        [caper_parser.Token.token_v, "saw"],
        [caper_parser.Token.token_det, "a"],
        [caper_parser.Token.token_n, "man"],
        [caper_parser.Token.token_prep, "in"],
        [caper_parser.Token.token_det, "the"],
        [caper_parser.Token.token_n, "park"],
        [caper_parser.Token.token_prep, "with"],
        [caper_parser.Token.token_det, "a"],
        [caper_parser.Token.token_n, "scope"],
        [caper_parser.Token.token_eof, "$"]
    ];
    var processed = [];
    var parser = new caper_parser.Parser(null);

    var context = null;
    $('#step-button').click(
        function() {
            if (context == null) {
                if (0 < data.length) {
                    var d = data.shift();
                    processed.push(d);
                    context = parser.postFirst(d[0], d[1]);
                    $('#status').text('post "' + d[1] + '"');
                }
                return;
            } else {
                $('#status').text('');
            }
            if(parser.postNext(context)) {
                if (0 < context.actives.length) {
                    $('#status').text('reduced');
                } else {
                    $('#status').text('no more reduce');
                }
            } else {
                $('#status').text('shifted');
                context = null;
            }
            window.cy.remove('*');
            parser.draw({
                drawVertex : function(i, t, x, y) {
                    window.cy.add({ group: "nodes", data: { id: 'vertex'+i, name: t, faveColor: '#D88' }, position: {x: 100 + x * 100, y: 100 + y * 50} });
                },
                drawVertexEdge : function(i0, x0, y0, i1, x1, y1) {
                    window.cy.add({ group: "edges", data: { source: 'vertex'+i0, target: 'vertex'+i1, faveColor: '#D88' } });
                },
                drawNode : function(i, x, y, t) {
                    window.cy.add({ group: "nodes", data: { id: 'node'+i, name: t, faveColor: '#88D' }, position: {x: 100 + x * 100, y: 400 + y * 50} });
                },
                drawNodeEdge : function(i0, i1) {
                    window.cy.add({ group: "edges", data: { source: 'node'+i0, target: 'node'+i1, faveColor: '#88D' } });
                },
                drawVertexToNodeEdge : function(vi, ni) {
                    window.cy.add({ group: "edges", data: { source: 'vertex'+vi, target: 'node'+ni, faveColor: '#DDD' } });
                }
            }, context);

            var processedSpan = $('#processed');
            var waitingSpan = $('#waiting');
            processedSpan.text('');
            waitingSpan.text('');
            for (var i = 0 ; i < processed.length ; i++) {
                processedSpan.append(processed[i][1] + " ");
            }
            for (var i = 0 ; i < data.length ; i++) {
                waitingSpan.append(data[i][1] + " ");
            }
        });
}); // on dom ready