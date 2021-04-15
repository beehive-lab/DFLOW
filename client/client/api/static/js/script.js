window.addEventListener('load', function () {
    main()
})

window.timeseries = {};

function main() {
    fetch('/api/v1/profiling-data-keys')
        .then(response => response.json())
        .then(profilingDataKeys => {
            console.log(profilingDataKeys);
            // Initialise all keys.
            let profilingDataKey;
            for (let i = 0; i < profilingDataKeys.length; i++) {
                profilingDataKey = profilingDataKeys[i];
                window.timeseries[profilingDataKey] = new TimeSeries();
            }
            return profilingDataKeys
        })
        .then(profilingDataKeys => {
            // Add graph for each key
            let profilingDataKey;
            for (let i = 0; i < profilingDataKeys.length; i++) {
                profilingDataKey = profilingDataKeys[i];
                addSmoothieGraphToPage(profilingDataKey);
            }
            return profilingDataKeys;
        })
        .then(profilingDataKeys => {
            setInterval(function () {
                fetch('/api/v1/live-profiling-data/')
                    .then(response => response.json())
                    .then(all_data => {
                        console.log(all_data)
                        let cur_data;
                        for (let i = 0; i < profilingDataKeys.length; i++) {
                            cur_data = all_data[i]
                            console.log(cur_data)
                            if (
                                cur_data != null &&
                                cur_data.key != null &&
                                cur_data.value != null &&
                                cur_data.timestamp != null
                            ) {
                                window.timeseries[cur_data.key].append(cur_data.timestamp * 1000, cur_data.value);
                            }
                        }
                    })
            }, 1000);
        })
        .then(() => {
                fetch('/api/v1/sensor-data-keys')
                    .then(response => response.json())
                    .then(sensorDataKeys => {
                        console.log(sensorDataKeys);
                        // Initialise all keys.
                        let sensorDataKey;
                        for (let i = 0; i < sensorDataKeys.length; i++) {
                            sensorDataKey = sensorDataKeys[i];
                            window.timeseries[sensorDataKey] = new TimeSeries();
                        }
                        return sensorDataKeys
                    })
                    .then(sensorDataKeys => {
                        // Add graph for each key
                        let sensorDataKey;
                        for (let i = 0; i < sensorDataKeys.length; i++) {
                            sensorDataKey = sensorDataKeys[i];
                            addSmoothieGraphToPage(sensorDataKey);
                        }
                        return sensorDataKeys;
                    })
                    .then(sensorDataKeys => {
                        setInterval(function () {
                            fetch('/api/v1/live-sensor-data/')
                                .then(response => response.json())
                                .then(all_data => {
                                    console.log(all_data)
                                    let cur_data;
                                    for (let i = 0; i < sensorDataKeys.length; i++) {
                                        cur_data = all_data[i]
                                        console.log(cur_data)
                                        if (
                                            cur_data != null &&
                                            cur_data.key != null &&
                                            cur_data.value != null &&
                                            cur_data.timestamp != null
                                        ) {
                                            window.timeseries[cur_data.key].append(cur_data.timestamp * 1000, cur_data.value);
                                        }
                                    }
                                })
                        }, 1000);
                    });
            }
        );

}


function addSmoothieGraphToPage(dataKey) {
    let graphArea = document.getElementById('graph-area');

    let col = document.createElement('col');
    col.setAttribute('class', 'col')
    graphArea.appendChild(col)

    let card = document.createElement('div');
    card.setAttribute('class', 'card');
    col.appendChild(card)

    let canvasId = dataKey + '-canvas';
    let smoothieCanvas = document.createElement('canvas');
    smoothieCanvas.setAttribute('id', canvasId);
    smoothieCanvas.setAttribute('class', 'card-img-top');
    card.appendChild(smoothieCanvas)

    let cardBody = document.createElement('div');
    cardBody.setAttribute('class', 'card-body');
    card.appendChild(cardBody);

    let graphTitle = document.createElement('h8');
    graphTitle.setAttribute('class', 'card-title')
    graphTitle.innerHTML = dataKey;
    cardBody.appendChild(graphTitle);

    let smoothie = new window.SmoothieChart(
        {
            responsive: true,
            maxValueScale: 1.27,
            minValueScale: 1.27,
            tooltip: true
        }
    );
    smoothie.streamTo(document.getElementById(canvasId), 10000);
    smoothie.addTimeSeries(
        window.timeseries[dataKey],
        {strokeStyle: 'rgb(0, 255, 0)', lineWidth: 3}
    );
}
