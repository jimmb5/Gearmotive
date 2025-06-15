## Anturit

-Lämpötila (ulko/öljy)
    Anturi: https://www.tarvikemotti.com/tuote/moottorin-lampoanturi-m14/?srsltid=AfmBOopDFnyLPRIA705stKWIq5bpt7tPa32fwjFh2ALQijLvjmEIcYf4
    Liitin: https://finjector.com/fin/2_napainen_amp_jpt_runko_963040_3-p-750402-126
    Tätä käytettäisiin öljyn lämpötilan mittaukseen. Tarkoitetaanko ulkolämpötilalla öljyn pinnan yläpuolella vallitsevaa lämpötilaa? Sen mittaukseen menis varmaankin yhtälailla sama anturi.
    Hinta ~15€ + liitin ~2.5€

-Akselien nopeudet
    'laippa' kiinnityksellä: https://ttspeed.fi/littelfuse-hall-pyorintanopeusanturi-laippakiinnityksella
    toisenlainen kiinnitys: https://ttspeed.fi/cherry-hall-effect-pyorintanopeusanturi-millinen
    Hall anturi, analoginen 0-5vDC output. Näillä mittaus hammasrattaasta, pitää tietää ainoastaan hampaiden lkm. 
    Hinta ~60€

-Vääntövoima (load cell, jarru + vaihteisto)
    Anturi: https://www.amazon.com/CALT-DYLY-103-Compression-Tension-Pressure/dp/B0BYSLN766?th=1
    Tuohon tarvittis vielä kuitenkin ilmeisesti sen vahvistimen tms. Datasheettejä ei myöskään oikeen kattavia.
    Hinta tällä ~60€ + vahvistin (?)

    Täältä sais valmiin "paketin":
    https://www.loadstarsensors.com/ras1-build-and-buy.html
    eli S mallisen load cell anturin + vahvistin joka muuntais outputin suoraan analogiseksi 0.5-4.5V. Suht pätevän olonen kampe olisi äkkiseltään mutta hinta sen mukainen. Saatavilla esim 1133kg, 2267kg versiot.
    Hintaa kokopaketilla ~1000€,  



-Tärinä/värähtely penkissä/laatikossa
    -XYZ akseleiden kiihtyvyys anturi, esim : https://www.amazon.com/AITRIP-ADXL335-Accelerometer-Angular-Transducer/dp/B08Z3DF43Z?th=1 vähän lelu ehkä?
    Parempi: https://www.digikey.fi/fi/products/detail/analog-devices-inc/EVAL-ADXL354CZ/6557294?gad_source=1&gad_campaignid=19551511633&gbraid=0AAAAADrbLlgMZg2TT2jQbe5XdcL0aNACl&gclid=CjwKCAjwr5_CBhBlEiwAzfwYuDYCV7UXkmmgGqSzy34OCzozQlqL49MiB4_nkw3M2lnotWDabAU3uRoCqMoQAvD_BwE&gclsrc=aw.ds 

    ADXL335 Output 0-5v tai ADXL354 0-3.3V, input 3.3VDC. Kattavat datasheetit molempiin.
    Kotelointi 3D tulostamalla? Osaaminen löytyisi.
    Hinta ADXL335 ~10€
    Hinta ADXL354 ~40€

 
-Voima-antureita ja niitten sijoittelua vaihteistoon?
    Antureiksi tähän teidän omia venymäliuskoja? Output 0-5VDC joten lukemissa ei ongelmaa. 

## Epäselvät anturit vielä, näissä useassa hinnat nousee herkästi 1000€+... Etsitäänkö halvempia vaihtoehtoja?

-Öljyn laatu/Sähkönjohtavuuden muutos ratasparien kuluessa
 - Metal particle detector
    esim. https://www.buehler-technologies.com/fileadmin/downloadcenter/DA150103_BMD.pdf 
    Hinta, saatavuus = ?
    Input oltava 18-33vDC joten ei ihanteellinen..

 - Contamination sensor
    Äkkiseltään löytyi tonneja maksavia kamppeita...

 - Quality sensor?
    Jokseenkin päteviä antureita: 
    https://gillsc.com/wp-content/uploads/2024/05/4212-Weardetect-Datasheet-Iss-11.pdf, monia outputteja
    https://www.tandeltasystems.com/wp-content/uploads/2020/01/23-09_TanDelta_DataSheet_OQSX-G2-Sensor_v1-1.pdf tässä myös lämpötilan logaus 

 - Conducticity sensor
    Erittäin huonosti valikoimaa myös tarjolla, lähinnä valmiita monen tonnin mittalaitteita.

-Öljyn mahdolliselle vaahtoamiselle jotain pitäis keksiä
    ?

-Öljyn määrä
    https://gillsc.com/compare-level-sensors/levelpro/
    Speksejä: https://gillsc.com/wp-content/uploads/2024/08/LevelPro-Datasheet-Iss-5.pdf 
    Eri mittaisia olemassa

-Öljyn esilämmitys?
    Mitä tällä haetaan takaa? 
    
-Ääni
    https://www.kouluelektroniikka.fi/tuote/ohjelmoitava-elektroniikka-ja-tarvikkeet/moduulit/aanimoduulit/4901063/electret-microphone-amplifier-max4466-with-adjus?srsltid=AfmBOooy8R22h6JgBkzN9dk_oFJ7exC17wlZcrCosryi6cd8aSCNq9SF
    Perus elektreettimikrofoni, 20Hz-20kHz, input 2.4-5VDC (3,3V ihanteellinen), output 
    Tällä siis ideana mitata tuleeko jotain piikkejä yleisesti?
    Hinta ~15€

-AE sensor, värähtelyt?
    Ei varsinaisesti AE sensori, mutta voisko ajaa asiansa?: https://www.ifm.com/fi/fi/product/VTV122?source=gs&gad_source=1&gad_campaignid=19932532946&gbraid=0AAAAApa2DSBvgUe1LKEWepWXUqpy2ReVy&gclid=CjwKCAjwr5_CBhBlEiwAzfwYuBXtf0cwmKUxRmAkgpEmQctp0JJ7M--oo3s1nDClyw5IFi2cFDyDlxoCJ2UQAvD_BwE 
    Mittaus 10-1000Hz, Pitää varmaan olla useita kymmeniä-satoja kHz?
    Output 4-20mA, input 9-32VDC eli ei tämän puolen ihanteellinen mutta loppujen lopuksi ei kaadu siihen

    Oikea AE sensor tarvittis ainakin vahvistimen, DC offsetoinnin ja suotimet, ei mahdottomuus mutta tulee viemään aikaa
 

## Mittavirheet

    Analogisella:
    Antureiden etäisyys esim 3m päässä, 0,2mm^2 paksuisella johdolla resistanssi ~84,2ohm/1000m eli noin 0,5ohm (3m+3m) tuolla matkalla. Anturit kuluttaa 1-5mA -> jännitehäviö 0.5mV ts. merkityksetön.

    Resistanssipohjaisella anturilla:
    Esimerkkinä lämpötila anturi jota suunnitellaan käytettäväksi projektissa. 3m etäiysyydellä 0.2mm^2 johdon resistanssi 0.5ohm. Ilman johdon lisäämää resistanssia 20 °C = 2200 ohm. Tuon johdon tuoman resistanssin jälkeen lukema on teoriassa 22,045 °C
    Myös merkityksetön.



