import Link from 'next/link'

let EmployeesPage = props => {
  return <>
    {props.bool ? <div className="flex flex-col fixed z-30 w-32 bg-black text-gray-200 top-18 rounded shadow select-none" style={{left: "14.78rem"}}>
      <Link href="/employees/filter/developers">
        <a className="rounded-t hover:bg-stone-900 py-2 text-center">Developers</a>
      </Link>
      <Link href="/employees/filter/designers">
        <a className="hover:bg-stone-900 text-sm py-2 text-center">Graphic Designers</a>
      </Link>
      <Link href="/employees/filter/finance">
        <a className="hover:bg-stone-900 py-2 text-center">Finance</a>
      </Link>
      <Link href="/employees/filter/writers">
        <a className="hover:bg-stone-900 py-2 text-center">Writers</a>
      </Link>
      <Link href="/employees">
        <a className="rounded-b hover:bg-stone-900 py-2 text-center">See all</a>
      </Link>
    </div>: <></>}
    {props.bool ? <div className="flex z-20 fixed top-0 right-0 left-0 bottom-0" onClick={()=>{props.set(false)}}></div>: <></>}
  </>
}

export default EmployeesPage