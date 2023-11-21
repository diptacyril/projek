from flask import Flask, render_template, request, redirect, url_for
from flask_mysqldb import MySQL
from datetime import datetime, timedelta

app = Flask(__name__)

# MySQL Configuration
app.config['MYSQL_HOST'] = 'localhost'
app.config['MYSQL_USER'] = 'root'
app.config['MYSQL_PASSWORD'] = 'root'
app.config['MYSQL_DB'] = 'parking_db'

mysql = MySQL(app)

# Function calculate per hour
def calculate_price(checkin_time, checkout_time):
    duration = checkout_time - checkin_time
    hours = duration.total_seconds() / 3600
    price_per_hour = 3000
    total_price = max(3000, round(hours * price_per_hour))
    return total_price

# Routes

@app.route('/')
def index():
    return render_template('checkin.html')

@app.route('/checkin', methods=['POST'])
def checkin():
    plate_number = request.form['plate_number']

    cur = mysql.connection.cursor()
    cur.execute("INSERT INTO parking_tickets (plate_number, checkin_time) VALUES (%s, %s)", (plate_number, datetime.now()))
    mysql.connection.commit()
    cur.close()

    return redirect(url_for('checkout', plate_number=plate_number))

@app.route('/checkout', methods=['GET', 'POST'])
def checkout():
    if request.method == 'POST':
        plate_number = request.form.get('plate_number')

        if plate_number:
            # Ambil informasi check-in dari database
            cur = mysql.connection.cursor()
            cur.execute("SELECT * FROM parking_tickets WHERE plate_number = %s AND checkout_time IS NULL", (plate_number,))
            ticket = cur.fetchone()
            cur.close()

            if ticket:
                checkin_time = ticket[2]  # Mengambil waktu check-in dari tuple (index 2)
                checkout_time = datetime.now()
                total_price = calculate_price(checkin_time, checkout_time)
                return render_template('checkout.html', plate_number=plate_number, checkin_time=checkin_time, checkout_time=checkout_time, total_price=total_price)
            else:
                return render_template('checkout.html', plate_number=plate_number, error_message="No check-in data available.")
    
    return render_template('checkout.html')

if __name__ == '__main__':
    app.run(debug=True)
