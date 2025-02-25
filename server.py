import os
import subprocess
import smtplib
from flask import Flask, request, jsonify, send_file, make_response
from werkzeug.utils import secure_filename
from flask_cors import CORS
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}}, supports_credentials=True)

UPLOAD_FOLDER = "uploads"
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

app.config["UPLOAD_FOLDER"] = UPLOAD_FOLDER
SMTP_SERVER = "smtp.gmail.com"
SMTP_PORT = 587
EMAIL_ADDRESS = "vivekrkdc2021bca@gmail.com"
EMAIL_PASSWORD = "mwekyvsndlneuglr"

# ------------------- COMPRESS FUNCTION ------------------- #
@app.route("/compress", methods=["POST"])
def compress():
    if "file" not in request.files:
        return jsonify({"error": "No file uploaded"}), 400

    file = request.files["file"]
    filename = secure_filename(file.filename)
    input_path = os.path.join(app.config["UPLOAD_FOLDER"], filename)
    output_path = input_path + ".huff"

    file.save(input_path)

    try:
        executable = "compress.exe"  # Windows executable
        subprocess.run([executable, input_path, output_path], check=True)

        return send_file(output_path, as_attachment=True)
    except subprocess.CalledProcessError as e:
        return jsonify({"error": str(e)}), 500
    except FileNotFoundError:
        return jsonify({"error": "compress.exe not found"}), 500


# ------------------- DECOMPRESS FUNCTION ------------------- #
@app.route("/decompress", methods=["POST"])
def decompress():
    if "file" not in request.files:
        return jsonify({"error": "No file uploaded"}), 400

    file = request.files["file"]
    filename = secure_filename(file.filename)
    input_path = os.path.join(app.config["UPLOAD_FOLDER"], filename)
    output_path = input_path.replace(".huff", "_decompressed.txt")

    file.save(input_path)

    try:
        executable = "decompress.exe"  # Windows executable
        subprocess.run([executable, input_path, output_path], check=True)

        return send_file(output_path, as_attachment=True)
    except subprocess.CalledProcessError as e:
        return jsonify({"error": str(e)}), 500
    except FileNotFoundError:
        return jsonify({"error": "decompress.exe not found"}), 500


# ------------------- ENCODE MESSAGE FUNCTION ------------------- #
@app.route("/encode", methods=["POST"])
def encode_message():
    data = request.json
    if "message" not in data:
        return jsonify({"error": "No message provided"}), 400

    message = data["message"]
    encoded_message = subprocess.run(["encode.exe", message], capture_output=True, text=True)
    
    if encoded_message.returncode != 0:
        return jsonify({"error": "Encoding failed"}), 500

    return jsonify({"encoded_message": encoded_message.stdout.strip()})


# ------------------- DECODE MESSAGE FUNCTION ------------------- #
@app.route("/decode", methods=["POST"])
def decode_message():
    data = request.json
    if "encoded_message" not in data:
        return jsonify({"error": "No encoded message provided"}), 400

    encoded_message = data["encoded_message"]
    decoded_message = subprocess.run(["decode.exe", encoded_message], capture_output=True, text=True)
    
    if decoded_message.returncode != 0:
        return jsonify({"error": "Decoding failed"}), 500

    return jsonify({"decoded_message": decoded_message.stdout.strip()})


# ------------------- SEND ENCODED MESSAGE FUNCTION ------------------- #
@app.route("/encode_and_send", methods=["OPTIONS", "POST"])
def encode_and_send():
    if request.method == "OPTIONS":
        response = make_response()
        response.headers["Access-Control-Allow-Origin"] = "*"
        response.headers["Access-Control-Allow-Methods"] = "POST, OPTIONS"
        response.headers["Access-Control-Allow-Headers"] = "Content-Type"
        return response

    data = request.json
    if "message" not in data or "email" not in data:
        return jsonify({"error": "Message and recipient email are required"}), 400

    recipient_email = data["email"]
    message = data["message"]

    # Run the encoding subprocess
    encoded_message = subprocess.run(["encode.exe", message], capture_output=True, text=True)

    if encoded_message.returncode != 0:
        return jsonify({"error": "Encoding failed"}), 500

    encoded_text = encoded_message.stdout.strip()

    # Send encoded message via email
    try:
        msg = MIMEMultipart()
        msg["From"] = EMAIL_ADDRESS
        msg["To"] = recipient_email
        msg["Subject"] = "Your Encoded Message"
        msg.attach(MIMEText(f"Here is your encoded message:\n\n{encoded_text}", "plain"))

        server = smtplib.SMTP(SMTP_SERVER, SMTP_PORT)
        server.starttls()
        server.login(EMAIL_ADDRESS, EMAIL_PASSWORD)
        server.sendmail(EMAIL_ADDRESS, recipient_email, msg.as_string())
        server.quit()

        return jsonify({"success": True, "encoded_message": encoded_text})  # ✅ Return encoded message as well
    except Exception as e:
        return jsonify({"error": str(e)}), 500



if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000,)
