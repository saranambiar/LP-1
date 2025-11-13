import streamlit as st

st.set_page_config("Seat Booking", layout = "centered")
ss = st.session_state
ss.setdefault("page", "signup")
ss.setdefault("users",{})
ss.setdefault("logged_in", False)
ss.setdefault("user", None)

def go(p):
    ss.page = p

st.sidebar.title("Navigate")
if st.sidebar.button("Sign up") : go("signup")
if st.sidebar.button("Login") : go("login")
if st.sidebar.button("Booking") : go("booking")
if st.sidebar.button("Logout"):
    ss.logged_in = False
    ss.user = None
    go("login")

st.title("Movie Ticket Booking App")

if ss.page == "signup":
    st.header("Create Account")
    with st.form("s"):
        u = st.text_input("Username")
        pw = st.text_input("Password", type = "password")
        pw2 = st.text_input("Confirm Password", type = "password")
        if st.form_submit_button("Sign Up"):
            if not u or not pw:
                st.warning("Enter username or password.")
            elif pw != pw2:
                st.warning("Passwords do not match.")
            elif u in ss.users:
                st.warning("Username already exists.")
            else:
                ss.users[u] = pw
                st.success("Account created successfully.")
                go("login")

    if(st.button("Go to login")):
        go("login")

elif ss.page == "login":
    st.header("Login")
    with st.form("l"):
        u = st.text_input("Username")
        pw = st.text_input("Password", type = "password")
        if st.form_submit_button("Login"):
            if not u or not pw:
                st.warning("Enter username or password.")
            elif u not in ss.users or ss.users[u] != pw:
                st.warning("Invalid credentials.")
            else:
                ss.logged_in = True
                ss.user = u
                go("booking")

elif ss.page == "booking":
    st.header("Movie Ticket Booking")

    if not ss.logged_in:
        st.warning("Please login to continue.")
        if st.button("Go to login"):
            go("login")
    else:
        st.subheader(f"Welcome, {ss.user}")

        movie = st.selectbox("Select Movie:", ["Avatar", "Dune 2", "A Beautiful Mind"])
        seats_req = st.selectbox("Number of seats:", list(range(1,11)))

        name = st.text_input("Full Name:")
        contact = st.text_input("Contact:")

        st.markdown("**Seat Selection:**")

        seats = [r+n for r in ["A","B","C"] for n in "12345"]

        pick = st.multiselect("Select seats:", seats)

        if st.button("Submit Booking Request"):
            final = pick[:seats_req]
            if len(final) != seats_req:
                st.warning(f"Select exactly {seats_req} seats.")
            elif not name or not contact:
                st.warning("Enter name and contact.")
            else:
                st.success("Booking successful!")
                st.write(f"**Movie:** {movie}")
                st.write(f"**Name:** {name}")
                st.write(f"**Contact:** {contact}")

        st.markdown("---")
        c1, c2 = st.columns(2)
        if c1.button("Logout"):
            ss.logged_in = False; ss.user = None; go("login")
        if c2.button("Back to Signup"):
            go("signup")