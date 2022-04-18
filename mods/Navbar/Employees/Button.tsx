import { ChevronDownIcon } from '@heroicons/react/solid'
import Link from 'next/link'

let EmployeesButton = props => {
  return props.set !== undefined ? <div className={`flex cursor-pointer space-x-2 items-center py-2 px-2 rounded-lg transition-colors ${props.bool ? "bg-black text-gray-200": "text-gray-800 hover:bg-stone-300"}`} onClick={()=>props.set(!props.bool)}>
    <span>Employees</span>
    <ChevronDownIcon className={`h-4 w-4 transition-transform ease-in-out ${props.bool ? "rotate-180": ""}`} />
  </div>: <Link href="/employees">
    <a className="flex cursor-pointer justify-center space-x-2 items-center py-2 px-2 rounded-lg transition-colors active:bg-black active:text-gray-200 text-gray-800 hover:bg-stone-300">
    Employees
    </a>
  </Link>
}

export default EmployeesButton