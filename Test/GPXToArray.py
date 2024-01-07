import xml.etree.ElementTree as ET

def leggi_file_gpx(filename):
    albero = ET.parse(filename)
    radice = albero.getroot()

    punti = []

    for trkpt in radice.findall(".//{http://www.topografix.com/GPX/1/1}trkpt"):
        lat = float(trkpt.get("lat"))
        lon = float(trkpt.get("lon"))
        ele = float(trkpt.findtext("{http://www.topografix.com/GPX/1/1}ele", default=0))
        time = trkpt.findtext("{http://www.topografix.com/GPX/1/1}time", default="")
        punti.append({"latitude": lat, "longitude": lon, "altitude": ele, "time": time})

    return punti

def genera_file_h(filename, array_name="CoordinateArray"):
    coordinate = leggi_file_gpx(filename)

    with open("Test/GPX_Points.h", "w") as f:
        f.write("#ifndef GPX_DATA_H\n")
        f.write("#define GPX_DATA_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write("typedef struct {\n")
        f.write("\tdouble latitude;\n")
        f.write("\tdouble longitude;\n")
        f.write("\tdouble altitude;\n")
        f.write("\tchar time[30]; // Modifica la dimensione se necessario\n")
        f.write("} GPSCoordinate;\n\n")
        
        f.write(f"static const uint32_t {array_name}_size = {len(coordinate)};\n\n")
        
        f.write(f"static const GPSCoordinate {array_name}[] =  {{\n")
        
        for punto in coordinate:
            f.write(f"\t{{ {punto['latitude']}, {punto['longitude']}, {punto['altitude']}, \"{punto['time']}\" }},\n")
        
        f.write("};\n\n")
        
        f.write("#endif // GPX_DATA_H\n")

def main():
    filename = "Test/gpxTest.gpx"
    genera_file_h(filename)

if __name__ == "__main__":
    main()
